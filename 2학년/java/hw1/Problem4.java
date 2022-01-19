import java.util.Scanner;

public class Problem4 {

	public static void main(String[] args) {
		int i,j,cnt=0,flg;
		String temp;
		char arr[] = new char[0];
		char string[] = new char[0];
		Scanner x = new Scanner(System.in);
		System.out.print("Enter a text: ");
		temp = x.nextLine();
		arr = temp.toCharArray();
		while(true) {
			System.out.print("Enter a string: ");
			temp = x.nextLine();
			if(temp.length()==0) System.out.println("You must enter a string.");
			else break;
		}
		string = temp.toCharArray();
		
		for(i=0;i<=arr.length-string.length;i++) {
			flg=1;
			for(j=0;j<string.length;j++) {
				if(arr[i+j] != string[j]) flg=0; 
			}
			if(flg==1) cnt++;
			
		}
		temp = String.valueOf(string);
		System.out.printf("There are %d instances of \"%s\"\n",cnt,temp);
		x.close();
	}

}
