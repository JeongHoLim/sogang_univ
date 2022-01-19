import java.util.*;
public class Problem3 {

	public static void main(String[] args) {
		
		char arr[];
		char alp;
		String input, temp;
		int cnt=0;
		Scanner x = new Scanner(System.in);
		System.out.print("Enter a text: ");
		input = x.nextLine();
		arr = input.toCharArray();
		while(true) {
			System.out.print("Enter a letter: ");
			temp = x.nextLine();
			if(temp.length()==1) break;
			else System.out.println("You must enter a single letter.");
		}
		alp = temp.charAt(0);
		for(int i=0;i<arr.length;i++) {
			if(arr[i] == alp) cnt++; 
		}
		System.out.printf("There are %d %c's in the text.\n",cnt,alp);
		x.close();
	}

}
