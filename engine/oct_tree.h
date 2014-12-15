/*oct_tree.h
 *
 *Mark Wilkes 100884169
 *
 *Oct-tree is an organizing system for objects in 3-d space
 *it as the name suggest is a base 8 where each of the children is a sector of the parent
 *the origin of each node is a representation of the top, front, left corner of the cube
 *the children are organized as follows:
 *
 *0th child is front, top, left
 *1st child is front, bottom, left
 *2nd child is front, top, right
 *3rd child is front, bottom, right
 *4th child is back, top, left
 *5th child is back, bottom, left
 *6th child is back, top, right
 *7th child is back, bottom, right
 *
 *this system uses recursive division of space to increase the precision of the space modelling
 */
 
/*
  *the spatial system will move with the player's craft centred in the root's cube
  *so this means it will be located on the origin of the roots 7th child
  *it, as it "moves", will apply the negation of its movement to every other object in the world
  *so the OctTree will not move and neither will the player but everything else will
  *this simplifies the updating of the OctTree by not requiring mass culling and additions to the tree
*/

/*
 for path find there is a list of pathing points which would normally be called either nodes or vertices but seeing as we
 already are using things with those names within the octtree they will be pathing points
 these pathing points are the centres of the lowest level nodes in the octtree (as defined as the node origin
			(x,y,z) times length/2)
 and each object that needs to path will either ask for a new goal point (which will be one of these pathing points)
 or already have one of these goal points

 the pathfinding objects will be using a spline to make there way there and along the way will be editting there path to
 not be running into things
 */
#pragma once

#include "ObjectModel.h"
#include "Oct_node.h"
#include <vector>
#include <algorithm>
#include <time.h>
#include "RVO.h"

using namespace std;

class Octtree{
	public:
		Octtree(XMFLOAT3 position, float length, int depth);
		~Octtree();
		
		int addObject(ObjectModel * newGameObject);

		int addPlayer(ObjectModel * player);
		
		virtual HRESULT drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager, Camera * camera);

		virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval);

	protected:
		RVO::RVOSimulator *rvoSim;
		Octnode * rootNode;
		int maxDepth;
		vector<ObjectModel *>* completeObjectList;

		ObjectModel * playerObj;

		virtual HRESULT refitting();
		
		int checkCollisions();
		int checkCollisionsBetween(Octnode* node1, Octnode* node2);
		int checkCollisionsWithin(Octnode* node);
		int checkUpTree(Octnode* currNode, Octnode* checkNode);
		int traverseTreeDown(Octnode* node);
		int checkCollisionsRay(vector<ObjectModel *>* outColliding, XMFLOAT3 posRay, XMFLOAT3 dirRay);
		int findNewGoal(ObjectModel* mover);

		void setupRVO();
		void addAsAgent(ObjectModel* newObj);
		void setPrefVelocity();
		bool reachedGoal(ObjectModel* obj);

		void updateSim();

		void trimTree();
		void trimBranch(Octnode* branch);
		void trimLeaf(Octnode* leaf);
};