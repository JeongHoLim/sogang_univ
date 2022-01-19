
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.Collections;
import java.util.Comparator;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
@SuppressWarnings({ "hiding", "serial" })
class MAP<String,Double> extends LinkedHashMap<String,Double>{
	
	public java.lang.String toString() {
		java.lang.String temp[] = super.toString().replace("=", " ").split(", ");
		
		java.lang.String arr = java.lang.String.join("\n", temp);
		return (java.lang.String) arr.substring(1, arr.length()-1);
	}
	
}
class MapManager{
	private static String line;
	private static BufferedReader br;
	private static Map<String,Double>map = new MAP<>();
	private static String arr[];
	private static List<Map.Entry<String, Double>> list;
	private static Map<String,Double> sortedMap;
	private static Map.Entry<String, Double> entry;
	private static int cmp;
	static Map<String,Double> readData(String fileName){
	
		try {
			br = new BufferedReader(new FileReader(fileName));
		}catch(FileNotFoundException e) {
			return null;
		}
		while(true) {
			try {
				line = br.readLine();
			}
			catch(IOException e) {
				e.printStackTrace();
			}
			if(line == null) break;
			arr = line.split(" ");
			map.put(arr[0],Double.parseDouble(arr[1]));
		}
		try {
			br.close();
		}catch(IOException e) {
			e.printStackTrace();
		}
		list = new LinkedList<>(map.entrySet());
		Collections.sort(list,new Comparator<Map.Entry<String, Double>>(){
			public int compare(Map.Entry<String, Double>o1,Map.Entry<String, Double>o2) {
				cmp = Double.compare(o1.getValue(), o2.getValue());
				return cmp==0?o1.getKey().compareTo(o2.getKey()) : cmp;
			}
		});
		sortedMap = new MAP<>();
		for(Iterator<Map.Entry<String, Double>> iter = list.iterator(); iter.hasNext();){
            entry = iter.next();
            sortedMap.put(entry.getKey(), entry.getValue());
        }
		return sortedMap;
	}	
}

public class Problem18 {
	public static void main(String[] args) {
		Map<String, Double> map = MapManager.readData("input.txt");
		if(map == null) {
			System.out.println("Input file not found.");
			return;
		}
		System.out.println(map);
	}
}
