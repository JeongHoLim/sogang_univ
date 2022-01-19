
interface Shape{
	public Double getArea();
}
class Circle implements Shape{

	private double r;
	Circle(double r){
		this.r = r;
	}
	public Double getArea(){
		return r*r*Math.PI;
	}
}

class Square implements Shape{

	private double r;
	Square(double r){
		this.r = r;
	}
	public Double getArea(){
		return r*r;
	}
}
class Rectangle implements Shape{

	private double r1,r2;
	Rectangle(double r1,double r2){
		this.r1 = r1;
		this.r2 = r2;
	}
	public Double getArea(){
		return r1*r2;
	}
}

public class Problem08{

	public static double sumArea(Shape arr[]){
		double sum=0;
		for(int i=0;i<arr.length;i++){
			sum += arr[i].getArea();
		}
		return sum;
	}	
	public static void main(String args[]){
		Shape[] arr = { new Circle(5.0), new Square(4.0),
				new Rectangle(3.0,4.0), new Square(5.0)};
		System.out.println("Total area of the shapes is: " + sumArea(arr));
	}


}




