import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;

class Item{
	
	private int cnt,i;
	private String word;
	private char [] temp;
	Item(String word,int cnt){
		this.word = word;
		this.cnt = cnt;
	}
	
	public String toString() { return this.word + " " +this.cnt;}
	String getWord() { return this.word;}
	int getCount() { return this.cnt;}
	void setCount() {
		this.cnt += 1;
	}
}

class MyFileReader{

	private static int i,j,flg=0,rem;
	private static char[] temp;
	private static BufferedReader br;
	private static String line;
	private static String[] arr;
	static boolean readDataFromFile(String file,ArrayList<Item> list) {
		try {
			br = new BufferedReader(new FileReader(file));
			while(true) {
				line = br.readLine();
				if(line == null) break;
				arr = line.split(" ");
				for(j=0;j<arr.length;j++) 
					Insert(list,arr[j]);	
			}
			
		}catch(FileNotFoundException e) {
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
	static void Insert(ArrayList<Item> list,String word) {
		temp = word.toCharArray();
			for(i=0;i<word.length();i++) {
				if(temp[i]>='A' && temp[i]<='Z') {
					temp[i] += 32;
				}
			}
			word = new String(temp);
	
		if(list.size() == 0) {
			list.add(new Item(word,1));
		}
		else {
			flg=0;

			for(i=0;i<list.size();i++) {
				if(word.equals(list.get(i).getWord())) {
					list.get(i).setCount();
					flg=1;
				}
			}
			if(i==list.size() && flg==0) list.add(new Item(word,1));

		}
	}
	void add(Item item,ArrayList<Item> list) {
		list.add(item);
	}
	
}
public class Problem15 {

	public static void main(String[] args) {
		 ArrayList<Item> list = new ArrayList<>();
		 boolean rv = MyFileReader.readDataFromFile("input_prob15.txt", list);
		 if(rv == false) {
		 System.out.println("Input file not found.");
		 return;
		 }
		 for(Item it: list) System.out.println(it); 
	}

}
