import java.util.ArrayList;

//////////////////////////
// 	Riley Lanksear		//
// 	250885525			//
// 	Assignment4			//	
//////////////////////////


public class AVLTree implements AVLTreeADT{
	
	//initializing variables for constructor 
	private int size;
	private AVLTreeNode root;
	
	/*
	 * Constructor 
	 */
	public AVLTree() {
		
		root = new AVLTreeNode();
		size = 0;
		
	}
	
	
	/*
	 * SETROOT 
	 * sets the root
	 */
	public void setRoot(AVLTreeNode root) {
		this.root = root;
	}
	
	/*
	 * ROOT 
	 * returns the root
	 */
	public AVLTreeNode root() {
		return root;
	}
	
	/*
	 * ISROOT
	 * boolean returns true if it is the root or false otherwise
	 */
	public boolean isRoot(AVLTreeNode node) {
		if(node.isRoot())
			return true;
		else
			return false;
	}
	
	/*
	 * GETSIZE
	 * returns the size of the tree
	 */
	public int getSize() {
		return size;
	}
	
	/*
	 * GETNODE
	 * returns the node that is contained in the tree
	 */
	public AVLTreeNode get(AVLTreeNode node, int key) {
		
		//if the node is a leaf return the node
		if(node.isLeaf()) {
			return node;
		}
		//else if the given key is equal to te key of the node then also return node
		else if (key == node.getKey()) {
			return node;
		}	
		//else if the key is less than the key of the node then recursively call left of the node
		else if (key < node.getKey()) {
			return get(node.getLeft(), key);
		}
		//else recursively call right of the node
		else {
			return get(node.getRight(), key);
		}	
	}
	

	/*
	 * SMALLEST
	 * returns the smallest node in the tree
	 */
	public AVLTreeNode smallest(AVLTreeNode node) {
		//if the node is a leaf return the parent of that node
		if(node.isLeaf()) {
			return node.getParent();
		}
		//else recursively call left of the node because the smallest value is on the left of the tree
		else {
			return smallest(node.getLeft());
		}
	}
	
	/*
	 * PUT
	 * puts a node in a tree
	 */
	public AVLTreeNode put(AVLTreeNode node, int key, int data) throws TreeException{
		//get node and key
		AVLTreeNode p = get(node, key);
		
		//if it is internal then throw and exception
		if (p.isInternal()) {
			throw new TreeException("Tree Exception has been thrown.");
		}
		
		else {
			//increment the size
			size ++;
			
			//set the key and data
			p.setKey(key);
			p.setData(data);
			//create two nodes for left and right to be the children of p
			AVLTreeNode tempLeft = new AVLTreeNode(p);
			AVLTreeNode tempRight = new AVLTreeNode(p);
			
			//set them as the children 
			p.setLeft(tempLeft);
			p.setRight(tempRight);

			return p;
		}	
	}
	
	
	/*
	 * REMOVE
	 * removes a node from a tree or throws an exception 
	 */
	public AVLTreeNode remove(AVLTreeNode node, int key) throws TreeException {
		
		//initialize AVLTreeNodes for later use
		AVLTreeNode p, parentP, c, cPrime, s;
		//get node and key
		p = get(node, key);
		
		//if p is a leaf throw exception
		if (p.isLeaf()) {
			new TreeException("Tree Exception has been thrown.");
		}
		
		else {
			//if either the left or the right of p is a lead then enter the first case
			if ( (p.getLeft().isLeaf()) || (p.getRight().isLeaf()) ) {			
				
				//get the parent of p
				parentP = p.getParent();
				//If the left child is not a leaf, then...
				if ( p.getLeft().isLeaf() == false ) {
					//decrement the size
					size --;
					//get the left child of p
					cPrime = p.getLeft();
					//if p is the root then set cPrime as the new root
					if (p.isRoot()) {
						setRoot(cPrime);
					}
					else {
						//set cPrimes parent as the parent of P
						cPrime.setParent(parentP);
						//if cPrime is equal to the right child of p then set parentPs right child as cPrime
						if(cPrime == p.getRight()) {
							parentP.setRight(cPrime);
						}
						else {
							//else set the left child of parentP to be cPrme
							parentP.setLeft(cPrime);
						}
					}
				}
				//else, the left child is a leaf. Then...
				else {
					//decrement size
					size --;
					
					//get the right child of p
					cPrime = p.getRight();	
					//if p is the root set root as cPrime
					if (p.isRoot()) {
						setRoot(cPrime);
					}
					else {
						//if it is not then set the parent of cPrime to the parent of P
						cPrime.setParent(parentP);
						//if cPrime is equal to the right child of p then set cPrime to be the parent of parentP
						if(cPrime == p.getRight()) {
							parentP.setRight(cPrime);
						}
						else {
							//else set the left child of parentP to be cPrime
							parentP.setLeft(cPrime);
						}
					}
				}
				return p;
			}
			
			//enter the second case of removal
			else { 
				
				//get the smallest value on the right side of p
				s = smallest(p.getRight());
				//set the data of that smallest node
				p.setData(s.getData());
				//set the key of that smallest node
				p.setKey(s.getKey());
				//remove the smallest node
				p = remove(s, s.getKey());
			}
		}
		return p;
	}
	
	
	/*
	 * INORDER
	 * creates a list and ten performs an inorder traversal 
	 */
	public ArrayList<AVLTreeNode> inorder(AVLTreeNode node){
		//creating an array list
		ArrayList temp = new ArrayList();
		//calling inorder rec on node and temp
		inorderRec(node, temp);
		return temp;
		
	}
	
	/*
	 * INORDERREC
	 * carries out the logic for the inorder traversal 
	 */
	public void inorderRec(AVLTreeNode node, ArrayList<AVLTreeNode> list) {
		
		if(!node.isLeaf()) {
			//if the left of node is not a lead then recursively call node.getleft
			if (!node.getLeft().isLeaf()) {
				inorderRec(node.getLeft(), list);
			}
			//add node to the list 
			list.add(node);
		
			//if the right is not a leaf then recursively call the right of node
			if (!node.getRight().isLeaf()) {
				inorderRec(node.getRight(), list);
			}
		}	
	}
	
	/*
	 * RECOMPUTE HEIGHT
	 * recomputes the height of a given node
	 */
	public void recomputeHeight(AVLTreeNode node) {
		
		node.setHeight( 1 + Math.max(node.getLeft().getHeight(), node.getRight().getHeight()));
		
	}



	/*
	 * PUT AVL
	 * puts a node into the tree and will re-balance if necessary 
	 */
	public void putAVL(AVLTreeNode node, int key, int data) throws TreeException {

	//	if(node.isInternal()) {
		//	throw new TreeException("Exception as been thrown");
//		}
		//create an avl tree node for put node, key and data
		AVLTreeNode r = put(node, key, data);
		//rebalance the tree
		rebalanceAVL(node, r);
			
		
		
	}

	/*
	 * REMOVE AVL
	 * removes a node from a tree and will re-balance if necessary
	 */
	public void removeAVL(AVLTreeNode node, int key) throws TreeException {
		
		//create an avl tree node for put node, key and data
		AVLTreeNode v =remove(node,key);
		//rebalance the tree
		rebalanceAVL(node, v);
		
	}


	/*
	 * REBALANCE AVL
	 * re-balances the tree if the height condition of the AVL is violated 
	 */
	public void rebalanceAVL(AVLTreeNode r, AVLTreeNode v) {
		
		//if v is not a leaf then recompute height
		if (!v.isLeaf()) {
			recomputeHeight(v);
		}
		
		//while v does not equal r 
		while(v != r) {
			
			//get the parent of v and set it as v
			v = v.getParent();
			
			//AVL tree condition check
			if ( Math.abs( v.getLeft().getHeight() - v.getRight().getHeight() ) > 1 ) {
				
				//declare variables 
				AVLTreeNode y, x, z; 
				//if the left side is greater than the right
				if (v.getLeft().getHeight() > v.getRight().getHeight()) {
					//set y to be the taller of v 
					y = taller(v, true);
				}
				else {
					y = taller(v, false);
				}
				
				z = v;
				x = taller(y, true); 	
				//call rotation 
				rotation(z,y,x);
			}
			//recompute the height
			recomputeHeight(v);
		}
	}

	/*
	 * TALLER
	 * returns the tallest sub tree of a given node
	 */
	public AVLTreeNode taller(AVLTreeNode node, boolean onLeft) {
		
		//set the height of left and the right into variables
		int leftHeight = node.getLeft().getHeight();
		int rightHeight = node.getRight().getHeight();
		
		//if the left is higher then return left of node
		if (leftHeight > rightHeight) {
			return node.getLeft();
		}
		//if right height is greater ten return right of node
		else if (rightHeight > leftHeight) {
			return node.getRight();
		}
		//if it is the root then return left of node
		else if (node.isRoot()) {
			return node.getLeft();
		}
		
		else {
			//if it is on the left return left of node
			if(onLeft == true) {
				return node.getLeft();
			}
			//return right of node
			else { 
				return node.getRight();
			}
		}
	}
	
	
	/*
	 * ROTATE
	 * rotates the tree if the tree is unbalanced 
	 */
	public AVLTreeNode rotate(AVLTreeNode y) {
		//get the parent of y
		AVLTreeNode z = y.getParent();
		//create boolean to indicate what rotation needs to be performed
		boolean rightRotation = nodeLeft(y);
		//set the parent of y to the parent of z
		y.setParent(z.getParent());
		//if z is the root then set the root as y
		if ( z.isRoot() == true) {
			setRoot(y);
		}
		//if z is on the left get the parent of y and set its left child to be y 
		else if (nodeLeft(z) == true) {
			y.getParent().setLeft(y);
		}
		//else set the right child of y to be parent of y
		else {
			y.getParent().setRight(y);
		}
		//if we need to perform a right rotation then carry out the updating of references 
		if (rightRotation == true) {
			z.setParent(y);
			z.setLeft(y.getRight());
			y.getRight().setParent(z);
			y.setRight(z);
		}
		//else it is a left rotation
		else {

			z.setParent(y);
			z.setRight(y.getLeft());
			y.getLeft().setParent(z);
			y.setLeft(z);
		}
		

		//recompute height twice 
		recomputeHeight(z);

		recomputeHeight(y);
		
		
		return y;
		

	}
	
	/*
	 * nodeLeft will return a boolean indicating what side the given node is on
	 */
	public boolean nodeLeft(AVLTreeNode node) {
				
		if (node.getParent().getLeft() == node) {
			return true;
		}
		
		else {
			return false;
		}
		
	}
		
	
	public AVLTreeNode rotation(AVLTreeNode z, AVLTreeNode y, AVLTreeNode x) {

		//if the right of z is equal to y and the right of y is equal to x then rotate on y
		if(z.getRight() == y && y.getRight() == x) {
			rotate(y);
			return y;
			
		}
		//if the left of z is equal to y and the left of y is equal to x then rotate on y
		else if (z.getLeft() == y && y.getLeft() == x) {
			rotate(y);
			return y; 
		}
		//else it is not a single rotation and we must rotate twice
		else {
			
			rotate(x);
			rotate(x);

			return x;
		}
	}	
}
