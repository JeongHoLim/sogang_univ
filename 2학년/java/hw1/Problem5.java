import java.util.*;
public class Problem5 {
	public static void main(String args[]) {
		int score[] = new int[5];
		int firstIdx=0,secondIdx=1;
		Scanner x = new Scanner(System.in);
		System.out.println("Enter exam scores of each student.");
		for(int i=0;i<5;i++) {
			System.out.print("Score of student "+(i+1)+": ");
			score[i] = x.nextInt();
			if(score[i]>=score[firstIdx]) {
				if(i>0) secondIdx = firstIdx;
				firstIdx = i;
			}
			else if(score[i]>=score[secondIdx])
				secondIdx = i;
		}
		
		System.out.printf("The 1st place is student %d with %d points.\n",firstIdx+1,score[firstIdx]);
		System.out.printf("The 2nd place is student %d with %d points.\n",secondIdx+1,score[secondIdx]);
		
		x.close();
	}
}
