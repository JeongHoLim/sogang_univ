import java.util.*;
public class Problem2 {

	public static void main(String[] args) {
		int i=1,low=1,high=100;
		int guess;
		Scanner x;
		int random = (int)(Math.random()*100)+1;
		while(true) {
			
			System.out.printf("[%d] Guess a number (%d-%d): ",i,low,high);
			x = new Scanner(System.in);
			guess = x.nextInt();
			if(guess >high || guess < low) {
				System.out.println("Not in range!");
			}
			else {
				if(guess == random) {
					System.out.println("Correct! Number of guesses: " + i);
					break;
				}
				else if(guess < random) {
					System.out.println("Too small!");
					low = guess+1;
				}
				else {
					System.out.println("Too large!");
					high = guess-1;
				}
				i++;
				
			}
			
			
			
		}
		x.close();
		
	}

}
