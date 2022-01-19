import java.util.*;
public class Problem1 {

	public static void main(String[] args) {
		
		String input;
		char alp;
		int len;
		int asc;
		System.out.print("ASCII code teller. Enter a letter: ");
		Scanner in = new Scanner(System.in);
		input = in.nextLine();
		len = input.length();
		if(len == 1) {
			alp = input.charAt(0);
			if((alp>='a' && alp<='z') || alp>='A' && alp<='Z') {
				asc = (int)(alp);
				System.out.println("The ASCII code of " + alp + " is " + asc +".");
			}
			else System.out.println("You must input a single uppercase or lowercase alphabet.");
		}
		else
			System.out.println("You must input a single uppercase or lowercase alphabet.");
	
		
		
		in.close();
	}

}
