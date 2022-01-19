import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;

class Element implements Comparable<Element>{
	
	private String name;
	private double price;
	Element(String name,double price ){ this.name = name; this.price = price;}
	
	double getPrice() { return this.price;}
	String getName() { return this.name;}
	public String toString() {
		return this.name+ " " + this.price;
	}
	public int compareTo(Element e) {
		if(this.price > e.getPrice()){ return 1;}
		if(this.price < e.getPrice()) return -1;
		return this.name.compareTo(e.getName());
	}
}
class ElementReader{
	private static String line;
	private static BufferedReader br;
	private static ArrayList<Element> list = new ArrayList<Element>();
	private static String arr[];
	static ArrayList<Element> readElements(String fileName) {
		try {
			br = new BufferedReader(new FileReader(fileName));
		}catch(FileNotFoundException e) {
			return null;
		}
		while(true) {
			try {
			line = br.readLine();
			}catch(IOException e) {
				e.printStackTrace();
			}
			if(line == null) break;
			arr = line.split(" ");
			list.add(new Element(arr[0],Double.parseDouble(arr[1])));

		}
		try {
			br.close();
		}catch(IOException e) {
			e.printStackTrace();
		}
		return list;
	}
	
	
}
public class Problem16 {

	public static void main(String[] args) {
		ArrayList<Element> list = ElementReader.readElements("input.txt");
		if(list == null) {
		System.out.println("Input file not found.");
		return;
		}
		Collections.sort(list);
		Iterator<Element> it = list.iterator();
		while(it.hasNext()) System.out.println(it.next());
	}

}
