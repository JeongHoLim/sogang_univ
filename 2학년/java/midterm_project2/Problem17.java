/*import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.Map;
import java.util.TreeMap;
@SuppressWarnings({ "hiding", "serial" })
class MAP<String,Double> extends TreeMap<String,Double>{
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
	static Map<String,Double> readData(String fileName){
	
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
			map.put(arr[0],Double.parseDouble(arr[1]));

		}
		try {
			br.close();
		}catch(IOException e) {
			e.printStackTrace();
		}
		return map;
	}
	
}

public class Problem17 {
	public static void main(String[] args) {
		Map<String, Double> map = MapManager.readData("input.txt");
		if(map == null) {
			System.out.println("Input file not found.");
			return;
		}
		System.out.println(map);
	}
}
*/