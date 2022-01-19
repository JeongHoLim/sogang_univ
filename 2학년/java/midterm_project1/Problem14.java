import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;

class Fruit{
	private String name;
	private double price;
	Fruit(String name,double price){
		this.name = name; this.price = price;
	}
	public String toString() {
		return this.name+" "+ this.price; 
	}
	String getName() { return this.name;	}
	Double getPrice() { return this.price;}
}
class FruitBox<T>{
	private int i,maxIdx,minIdx;
	private double sum,maxPr,minPr;
	private Fruit s;
	private ArrayList<T> list  = new ArrayList<T>();
	
	String getMaxItem(){
		maxPr = ((Fruit)list.get(0)).getPrice();
		maxIdx=0;
		for(i=1;i<list.size();i++) {
			s = (Fruit)(list.get(i));
			if(s.getPrice()>maxPr) {
				maxPr = s.getPrice();
				maxIdx = i;
			}
		}
		s = (Fruit)(list.get(maxIdx));
		return s.getName();
	}
	
	String getMinItem(){
		minPr = ((Fruit)list.get(0)).getPrice();
		minIdx=0;
		for(i=1;i<list.size();i++) {
			s = (Fruit)(list.get(i));
			if(s.getPrice()<minPr) {
				minPr = s.getPrice();
				minIdx = i;
			}
		}
		s = (Fruit)(list.get(minIdx));
		return s.getName();
	
	}
	double getMaxPrice() { 		return this.maxPr;	}
	double getMinPrice() {		return this.minPr;	}
	double getAvgPrice(){
		sum=0;
		for(i=0;i<list.size();i++) {
			s = (Fruit)(list.get(i));
			sum += s.getPrice();
		}
		
		return sum/list.size();
	}
	int getNumItems(){	return list.size();	}
	void add(T item) {	
		list.add(item);
		System.out.println(item);
		}
	T get(int i) {	return list.get(i);	}
}
class ItemReader{
	
	private static BufferedReader br;
	private static String line;
	private static String arr[];
	static boolean fileToBox(String file,FruitBox<Fruit> box) {
	
		try {
			br = new BufferedReader(new FileReader(file));
			while(true) {
				line = br.readLine();
				if(line == null) break;
				arr = line.split(" ");
				box.add(new Fruit(arr[0],Double.parseDouble(arr[1])));
			}
		}catch(FileNotFoundException e) {
			System.out.println("Input file not found.");
			return false;
		}catch(IOException e) {
			e.printStackTrace();
		}
		try{
			br.close();
		}catch(IOException e) {
			e.printStackTrace();
		}
		
		return true;
	}
	
}
public class Problem14 {

	public static void main(String[] args) {
		 FruitBox<Fruit> box = new FruitBox<>();
		 boolean rv = ItemReader.fileToBox("input_prob14.txt", box);
		 if(rv == false) return;
		 box.add(new Fruit("orange", 9.99));
	
		 System.out.println("----------------");
		 System.out.println(" Summary");
		 System.out.println("----------------");
		 System.out.println("number of items: " + box.getNumItems());
		 System.out.println("most expensive item: " + box.getMaxItem() + " (" +
		 box.getMaxPrice() + ")");
		 System.out.println("cheapest item: " + box.getMinItem() + " (" +
		 box.getMinPrice() + ")");
		 System.out.printf("average price of items: %.2f", box.getAvgPrice());

	}

}
