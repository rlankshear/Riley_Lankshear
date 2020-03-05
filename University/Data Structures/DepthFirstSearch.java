import java.util.ArrayList;
import java.util.Iterator;
import java.util.Stack;

//////////////////////////
//	Riley Lanksear		//
//	250885525			//
//	Assignment4			//	
//////////////////////////


public class DepthFirstSearch {

	//declaring variables
	RouteGraph inputGraph;
	Stack<Intersection> stack;
	
	/*
	 * Constructor 
	 */
	public DepthFirstSearch(RouteGraph graph) {
		
		inputGraph = graph;
		stack = new Stack<Intersection>();
		
	}
	/*
	 * Path --> calls pathRec and will return the stack
	 */
	public Stack<Intersection> path(Intersection startVertex, Intersection endVertex) throws GraphException {
			
		pathRec(startVertex, endVertex);
		
		return stack;
	}
	
	/*
	 * pathRec --> Identifies different possible paths
	 */
	public boolean pathRec(Intersection startVertex, Intersection endVertex) throws GraphException {
		
		//mark the startVertex 
		startVertex.setMark(true);
		//push it onto the stack
		stack.push(startVertex);
		
		//check to see if the label of startVertex is equal to that of endVertex
		if(startVertex.getLabel() == endVertex.getLabel()) {
			return true;
		}
		//if it is not then create an iterator of type road
		else {
			Iterator<Road> edges = inputGraph.incidentRoads(startVertex);
			//enter while loop while it has edges 
			while (edges.hasNext()) {
				Road road = edges.next();
				//if the road.getFirstEndPoint has not been marked then return true
				if(!road.getFirstEndpoint().getMark()) {
					if (pathRec(road.getFirstEndpoint(), endVertex)) {	
						return true;
					}
				}
				// else if the second end point has not been marked --> return true
				else if (!road.getSecondEndpoint().getMark()) {
					if (pathRec(road.getSecondEndpoint(), endVertex)) {
						return true;
					}
				}
			}
			//pop the stack and return false
			stack.pop();
			return false;
		}
	}
}
