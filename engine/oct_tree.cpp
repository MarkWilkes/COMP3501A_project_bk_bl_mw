#include "oct_tree.h"
#include "ObjectModel.h"

Octtree::Octtree(XMFLOAT3 position, float length, int depth) : rvoSim(0), playerObj(0), ticksForCollision(0){
	maxDepth = depth;
	rootNode = new Octnode(position, length, depth, 0, NULL);
	completeObjectList = new vector<ObjectModel *>();
	setupRVO();
}

Octtree::~Octtree(){
	delete rootNode;

	if (completeObjectList != 0){
		delete completeObjectList;
		completeObjectList = 0;
	}

	if (rvoSim != 0){
		delete rvoSim;
		rvoSim = 0;
	}
}

int Octtree::addPlayer(ObjectModel * player){
	if (player->type == ObjectType::PlayerShip){
		if (addObject(player) == 0){
			playerObj = player;
			return 0;
		}
	}
	return -1;
}

int Octtree::addObject(ObjectModel * newGameObject){
	//start a recursive call to put the object in the tree
	if(rootNode->fits(newGameObject) == 0){
		//only add the object to the list if it is put into the list at some point
		completeObjectList->push_back(newGameObject);
		//give it a goal
		addAsAgent(newGameObject);
		findNewGoal(newGameObject);
		
		return 0;
	}
	//something went wrong and the object doesn't fit anywhere in the tree
	return -1;
}

int Octtree::checkCollisions(){
	/*
	iterate through the tree starting at the deepest children and going up the tree checking for collisions
	*/
	traverseTreeDown(rootNode);
	return 0;
}

int Octtree::checkCollisionsBetween(Octnode* node1, Octnode* node2){
	//check all objects in the 2 passed nodes for collisions and pass back a list of all the collisions
	for (size_t i = 0; i < node1->nodeObjectList->size(); i++){
		for (size_t j = 0; j < node2->nodeObjectList->size(); j++){
			//sphere vs sphere collision checks
			//distace between the spheres
			float distBetween = sqrt(	pow(node1->nodeObjectList->at(i)->getBoundingOrigin().x - 
										node2->nodeObjectList->at(j)->getBoundingOrigin().x, 2) +
										pow(node1->nodeObjectList->at(i)->getBoundingOrigin().y -
										node2->nodeObjectList->at(j)->getBoundingOrigin().y, 2) +
										pow(node1->nodeObjectList->at(i)->getBoundingOrigin().z -
										node2->nodeObjectList->at(j)->getBoundingOrigin().z, 2)
				);
			//check if distance is less than the sum of the radiis
			if (distBetween < (	node1->nodeObjectList->at(i)->getBoundingRadius() +
								node2->nodeObjectList->at(j)->getBoundingRadius())){
				//if they are colliding add them to the list of pairs of collisions
				/*
				ObjectModel **newCollision = new ObjectModel*[2];
				newCollision[0] = node1->nodeObjectList->at(i);
				newCollision[1] = node2->nodeObjectList->at(j);
				outCollisions->push_back(newCollision);
				*/
				if (node1->nodeObjectList->at(i)->type == ObjectType::MineShip ||
					node2->nodeObjectList->at(j)->type == ObjectType::MineShip){
					//extra damage
					if (node1->nodeObjectList->at(i)->hasCollided() && !node2->nodeObjectList->at(j)->hasCollided()){
						//j hasn't collided and i has
						node2->nodeObjectList->at(j)->collideWith(node1->nodeObjectList->at(i)->type);
					}
					else if (!node1->nodeObjectList->at(i)->hasCollided() && node2->nodeObjectList->at(j)->hasCollided()){
						//j has collided and i hasn't
						node1->nodeObjectList->at(i)->collideWith(node2->nodeObjectList->at(j)->type);
					}
					else if (!node1->nodeObjectList->at(i)->hasCollided() && !node2->nodeObjectList->at(j)->hasCollided()){
						//neither has collided
						node2->nodeObjectList->at(j)->collideWith(node1->nodeObjectList->at(i)->type);
						node1->nodeObjectList->at(i)->collideWith(node2->nodeObjectList->at(j)->type);
					}
				}
				else{
					//regular damage to each
					if (node1->nodeObjectList->at(i)->hasCollided() && !node2->nodeObjectList->at(j)->hasCollided()){
						//j hasn't collided and i has
						node2->nodeObjectList->at(j)->collideWith(node1->nodeObjectList->at(i)->type);
					}
					else if (!node1->nodeObjectList->at(i)->hasCollided() && node2->nodeObjectList->at(j)->hasCollided()){
						//j has collided and i hasn't
						node1->nodeObjectList->at(i)->collideWith(node2->nodeObjectList->at(j)->type);
					}
					else if (!node1->nodeObjectList->at(i)->hasCollided() && !node2->nodeObjectList->at(j)->hasCollided()){
						//neither has collided
						node2->nodeObjectList->at(j)->collideWith(node1->nodeObjectList->at(i)->type);
						node1->nodeObjectList->at(i)->collideWith(node2->nodeObjectList->at(j)->type);
					}
				}
			}
		}
	}
	return 0;
}

int Octtree::checkCollisionsWithin(Octnode* node){
	//check for all possible collisions between this node and all other nodes
	for (std::vector<ObjectModel*>::size_type i = 0; i < node->nodeObjectList->size(); i++){
		for (std::vector<ObjectModel*>::size_type j = i; j < node->nodeObjectList->size(); j++){
			if (i == j){
				//let's stop checking collisions on our self shall we
				continue;
			}
			//sphere vs sphere collision checks
			//distace between the spheres
			float distBetween = sqrt(pow(node->nodeObjectList->at(i)->getBoundingOrigin().x -
				node->nodeObjectList->at(j)->getBoundingOrigin().x, 2) +
				pow(node->nodeObjectList->at(i)->getBoundingOrigin().y -
				node->nodeObjectList->at(j)->getBoundingOrigin().y, 2) +
				pow(node->nodeObjectList->at(i)->getBoundingOrigin().z -
				node->nodeObjectList->at(j)->getBoundingOrigin().z, 2)
				);
			//check if distance is less than the sum of the radiis
			if (distBetween < (node->nodeObjectList->at(i)->getBoundingRadius() +
				node->nodeObjectList->at(j)->getBoundingRadius())){
				if (node->nodeObjectList->at(i)->type == ObjectType::MineShip ||
					node->nodeObjectList->at(j)->type == ObjectType::MineShip){
					//extra damage
					if (node->nodeObjectList->at(i)->hasCollided() && !node->nodeObjectList->at(j)->hasCollided()){
						//j hasn't collided and i has
						node->nodeObjectList->at(j)->collideWith(node->nodeObjectList->at(i)->type);
					}
					else if (!node->nodeObjectList->at(i)->hasCollided() && node->nodeObjectList->at(j)->hasCollided()){
						//j has collided and i hasn't
						node->nodeObjectList->at(i)->collideWith(node->nodeObjectList->at(j)->type);
					}
					else if (!node->nodeObjectList->at(i)->hasCollided() && !node->nodeObjectList->at(j)->hasCollided()){
						//neither has collided
						node->nodeObjectList->at(j)->collideWith(node->nodeObjectList->at(i)->type);
						node->nodeObjectList->at(i)->collideWith(node->nodeObjectList->at(j)->type);
					}
				}
				else{
					//regular damage to each
					if (node->nodeObjectList->at(i)->hasCollided() && !node->nodeObjectList->at(j)->hasCollided()){
						//j hasn't collided and i has
						node->nodeObjectList->at(j)->collideWith(node->nodeObjectList->at(i)->type);
					}
					else if (!node->nodeObjectList->at(i)->hasCollided() && node->nodeObjectList->at(j)->hasCollided()){
						//j has collided and i hasn't
						node->nodeObjectList->at(i)->collideWith(node->nodeObjectList->at(j)->type);
					}
					else if (!node->nodeObjectList->at(i)->hasCollided() && !node->nodeObjectList->at(j)->hasCollided()){
						//neither has collided
						node->nodeObjectList->at(j)->collideWith(node->nodeObjectList->at(i)->type);
						node->nodeObjectList->at(i)->collideWith(node->nodeObjectList->at(j)->type);
					}
				}
			}
		}
	}
	return 0;
}

int Octtree::checkUpTree(Octnode* currNode, Octnode* checkNode){
	//check for all collisions between these nodes
	checkCollisionsBetween(currNode, checkNode);

	//so long as you didn't just check against the root check call this on the parent
	if (currNode->depthMe > 0){
		checkUpTree(currNode->parentNode, checkNode);
	}
	return 0;
}

int Octtree::traverseTreeDown(Octnode* node){
	for (int i = 0; i < 8; i++){
		if (node->children[i] == NULL){
			//this is bottom node
		}
		else{
			checkUpTree(node, node->children[i]);
			traverseTreeDown(node->children[i]);
		}
		checkCollisionsWithin(node);
	}
	return 0;
}


HRESULT Octtree::drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager, Camera * camera) {
	for (std::vector<ObjectModel*>::size_type i = 0; i < completeObjectList->size(); i++){
		if (FAILED((*completeObjectList)[i]->draw(context, manager, camera))){
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	return ERROR_SUCCESS;
}

HRESULT Octtree::update(const DWORD currentTime, const DWORD updateTimeInterval) {
	HRESULT result = ERROR_SUCCESS;
	updateSim();
	
	for (std::vector<ObjectModel*>::size_type i = 0; i < completeObjectList->size(); i++){
		if (FAILED(((*completeObjectList)[i]->updateContainedTransforms(currentTime, updateTimeInterval)))){
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			return result;
		}
	}
	
	if ((++ticksForCollision %= 5) == 0){
		result = refitting();

		checkCollisions();

		for (size_t i = 0; i < completeObjectList->size(); i++){
			if (completeObjectList->at(i)->hasCollided()){
				completeObjectList->at(i)->takeDamage();
			}
		}

		trimTree();

		vector<ObjectModel*>::iterator curr = completeObjectList->begin();
		while (curr != completeObjectList->end()){
			if ((*curr)->isDead()){
				size_t agentNum = (unsigned int)(*curr)->getAgentNum();
				rvoSim->setAgentPosition(agentNum, RVO::Vector3(0, 0, 0));
				rvoSim->setAgentRadius(agentNum, 0);
				rvoSim->setAgentPrefVelocity(agentNum, RVO::Vector3(0, 0, 0));
				delete (*curr);
				curr = completeObjectList->erase(curr);
			}
			else{
				curr++;
			}
		}
	}
	return result;
}

HRESULT Octtree::refitting(){
	vector<ObjectModel*>* newList = new vector<ObjectModel*>();

	HRESULT result = ERROR_SUCCESS;

	//call the recursive function to find all the objects that don't fit in the node any more
	rootNode->checkObjectUpdates(newList);

	//find them a new home or they don't need to be around anymore
	for (size_t i = 0; i < newList->size(); i++){
		//can't fit it anymore so it needs to get gone
		if (rootNode->fits((*newList)[i]) == -1){
			//find the not so fitting object in the master list
			vector<ObjectModel*>::iterator loc = find(completeObjectList->begin(), completeObjectList->end(), (*newList)[i]);
			//make sure we found it in case of some magic happening (read as something breaking and going unnoticed)
			if (loc != completeObjectList->end()){
				delete *loc;
				completeObjectList->erase(loc);
			}
			else{
				result = ERROR_DATA_NOT_FOUND;
			}
		}
	}

	delete newList;

	return result;
}

/*
	A note of this function, you will return the list of all the collisions including the object firing the laser
	so you have to handle the list of all the return
*/
int Octtree::checkCollisionsRay(vector<ObjectModel *>* outCollsion, XMFLOAT3 posRay, XMFLOAT3 dirRay){
	int result = -1;
	
	for (size_t i = 0; i < completeObjectList->size(); i++){
		XMFLOAT3 p = XMFLOAT3	(posRay.x - completeObjectList->at(i)->getBoundingOrigin().x,
								posRay.y - completeObjectList->at(i)->getBoundingOrigin().y,
								posRay.z - completeObjectList->at(i)->getBoundingOrigin().z);
		float b = -((p.x * dirRay.x) + (p.y * dirRay.y) + (p.z * dirRay.z));
		float det = b*b - ((p.x * p.x)+(p.y * p.y)+(p.z * p.z)) + pow(completeObjectList->at(i)->getBoundingRadius(),2);

		if (det < 0) continue;
		det = sqrt(det);
		float t1 = b - det;
		float t2 = b + det;

		if (t2 < 0) continue;
		outCollsion->push_back(completeObjectList->at(i));
		result = 0;
	}
	
	return result;
}


int Octtree::findNewGoal(ObjectModel* obj){
	//TEMP make new random goal
	if (obj->type == ObjectType::Other || obj->type == ObjectType::PlayerShip || obj->type == ObjectType::Asteroid){
		obj->updateGoalPos(obj->getBoundingOrigin());
		return 0;
	}
	else if(obj->type == ObjectType::GalleonShip){
		obj->updateGoalPos(XMFLOAT3((float)(rand() % 100) - 50, (float)(rand() % 100) - 50, (float)(rand() % 100) - 50));
		return 0;
	}
	else if (obj->type == ObjectType::MineShip){
		//TODO
		obj->updateGoalPos(XMFLOAT3((float)(rand() % 100) - 50, (float)(rand() % 100) - 50, (float)(rand() % 100) - 50));
		return 0;
	}
	else if (obj->type == ObjectType::EnemyShip){
		//TODO
		obj->updateGoalPos(XMFLOAT3((float)(rand() % 100) - 50, (float)(rand() % 100) - 50, (float)(rand() % 100) - 50));
		return 0;
	}

	return -1;
}

void Octtree::setupRVO(){
	rvoSim = new RVO::RVOSimulator();

	rvoSim->setTimeStep(0.125f);

	rvoSim->setAgentDefaults(20.0f, 10, 0.5f, 1.0f, 5.0f);
}

void Octtree::addAsAgent(ObjectModel* newObject){
	RVO::Vector3 &vec = RVO::Vector3(newObject->getBoundingOrigin().x, newObject->getBoundingOrigin().y, newObject->getBoundingOrigin().z);
	int numAgent = rvoSim->getNumAgents();
	newObject->setAgentNum(numAgent);

	rvoSim->addAgent(vec);
}

void Octtree::setPrefVelocity(){
	for (size_t i = 0; i < completeObjectList->size(); i++){
		RVO::Vector3 goal = RVO::Vector3(completeObjectList->at(i)->getGoalPos().x,
			completeObjectList->at(i)->getGoalPos().y,
			completeObjectList->at(i)->getGoalPos().z);

		RVO::Vector3 goalVec = goal - rvoSim->getAgentPosition(completeObjectList->at(i)->getAgentNum());

		if (RVO::absSq(goalVec)>1.0f){
			goalVec = RVO::normalize(goalVec);
		}

		rvoSim->setAgentPrefVelocity(completeObjectList->at(i)->getAgentNum(), goalVec);
	}
}

bool Octtree::reachedGoal(ObjectModel* obj){
	RVO::Vector3 goal = RVO::Vector3(obj->getGoalPos().x, obj->getGoalPos().y, obj->getGoalPos().z);

	if (RVO::absSq(rvoSim->getAgentPosition(obj->getAgentNum()) - goal) > 3*obj->getBoundingRadius()){
		return false;
	}
	findNewGoal(obj);
	return true;
}

void Octtree::updateSim(){
	setPrefVelocity();
	rvoSim->doStep();
	for (size_t i = 0; i < completeObjectList->size(); i++){
		if (completeObjectList->at(i)->type == ObjectType::PlayerShip){
			XMFLOAT3 playerPos = completeObjectList->at(i)->getBoundingOrigin();
			RVO::Vector3 playerCurrPos(playerPos.x, playerPos.y, playerPos.z);
			rvoSim->setAgentPosition(completeObjectList->at(i)->getAgentNum(),playerCurrPos);
			continue;
		}
		if(reachedGoal(completeObjectList->at(i))){
			
		}
		else{
			XMFLOAT3 posInOb = completeObjectList->at(i)->getBoundingOrigin();
			RVO::Vector3 posSim = rvoSim->getAgentPosition(completeObjectList->at(i)->getAgentNum());
			XMFLOAT3 posSimXM = XMFLOAT3(posSim.x(), posSim.y(), posSim.z());

			completeObjectList->at(i)->updateMoveToPos(posSimXM);
		}
	}
}


void Octtree::trimTree(){
	trimBranch(rootNode);
}

void Octtree::trimBranch(Octnode* branch){
	for (size_t i = 0; i < sizeof(branch->children)/sizeof(Octnode*); i++){
		if (branch->children[i] != NULL){
			trimBranch(branch->children[i]);
		}
	}
	trimLeaf(branch);
}

void Octtree::trimLeaf(Octnode* leaf){
	vector<ObjectModel*>::iterator curr = leaf->nodeObjectList->begin();
	while (curr != leaf->nodeObjectList->end()){
		if ((*curr)->isDead()){
			curr = leaf->nodeObjectList->erase(curr);
		}
		else{
			curr++;
		}
	}
}