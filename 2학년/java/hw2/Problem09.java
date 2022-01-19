class Point{
	
	private double arr[];
	Point(double arr[]){
		this.arr = arr;
	}
	public double[] getArray() {
		return this.arr;		
	}
	
}
class EuclideanDistance{
	private static double d=0;
	private static int i;
	private static double arr1[],arr2[];
	public static double getDist(Point x,Point y){
		arr1 = x.getArray();
		arr2 = y.getArray();
		if(arr1.length != arr2.length) return -1;
		
		for(i=0;i<arr1.length;i++) {
			d += Math.pow((arr1[i]-arr2[i]),2);
		}
		return Math.sqrt(d);
	}	
}
class ManhattanDistance {

	private static double d=0;
	private static int i;
	private static double arr1[],arr2[];
	
	public static double getDist(Point x,Point y){	

		arr1 = x.getArray();
		arr2 = y.getArray();
		if(arr1.length != arr2.length) return -1;
		for(i=0;i<arr1.length;i++) {
			d += Math.abs(arr1[i]-arr2[i]);
		}
		return d;
	}
}
public class Problem09{

	public static void main(String args[]){
		Point p1 = new Point(new double[]{1.0,2.0,3.0});
		Point p2 = new Point(new double[]{4.0,5.0,6.0});
		System.out.println("Euclidean Distance: " + EuclideanDistance.getDist(p1,p2));
		System.out.println("Manhattan Distance: " + ManhattanDistance.getDist(p1,p2));
		Point p3 = new Point(new double[]{1.0,2.0,3.0});
		Point p4 = new Point(new double[]{4.0,5.0});
		System.out.println("Euclidean Distance: " + EuclideanDistance.getDist(p3,p4));
		System.out.println("Manhattan Distance: " + ManhattanDistance.getDist(p3,p4));

	}
	
	
	
	
}