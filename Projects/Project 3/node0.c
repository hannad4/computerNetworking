#include <stdio.h>

extern struct rtpkt {
  int sourceid;       /* id of sending router sending this pkt */
  int destid;         /* id of router to which pkt being sent 
                         (must be an immediate neighbor) */
  int mincost[4];    /* min cost to node 0 ... 3 */
  };

extern int TRACE;
extern int YES;
extern int NO;
extern float clocktime;	/* The clock to be used for the emulator */
struct rtpkt *pkt0; 	/* This packet will hold the minimum distances */

struct distance_table 
{
  int costs[4][4];
} dt0;


/* students to write the following two routines, and maybe some others */

void rtinit0() 
{
  int i, j; 
  pkt0 = (struct rtpkt *)malloc( sizeof(struct rtpkt) );
	pkt0->sourceid = 0;		/* pkt0->sourceid is this node which is 0 */

	printf("NODE0: initialized at t=%f\n", clocktime );		/* Notify of initialization */
	
	dt0.costs[0][0] = 0;
	dt0.costs[0][1] = 1;
	dt0.costs[0][2] = 3;
	dt0.costs[0][3] = 7;  // Defining the costs that are officially known

	for (i=1; i<4; i++)
	{ 	
		for (j=0; j<4; j++)
		{ 
			dt0.costs[i][j]=999;		/* Setting costs to "infinity" which is 999 */
		}
	} 


	pkt0->mincost[0] = 0;					 /* The cost of a node to itself is 0 */
	pkt0->mincost[1] = dt0.costs[1][1] = 1;  /* The cost to node 1 is 1 */
	pkt0->mincost[2] = dt0.costs[2][2] = 3;  /* The cost to node 2 is 3 */
	pkt0->mincost[3] = dt0.costs[3][3] = 7;  /* The cost to node 3 is 7 */

	printf("Distance table is:\n");
	printdt0(&dt0);	/* print the distance table */

	pkt0->sourceid = 0;			/* pkt0->sourceid is this node which is 0 */
	printf("NODE0: send new min event occurred at t=%f\n\n", clocktime );	/* Notify of event occuring */
	
	/* send to all directly attached nodes (1, 2, 3) */
	for(i=0; i<4; i++ ){
		if(i==0 ) continue;
		pkt0->destid = i;
		tolayer2(*pkt0);		/* tolayer2 is defined in prog3.c */
	}
}


void rtupdate0(rcvdpkt)
  struct rtpkt *rcvdpkt;
{
  int i, change = 0, via = rcvdpkt->sourceid;
	
	printf("\nNODE0: Update occurred at t=%f\n", clocktime );		/* Notify of update */

	for(i=0; i<4; i++ ){
		if(i==0 || i==via || rcvdpkt->mincost[i]==999) continue;

		if( pkt0->mincost[i] > (dt0.costs[i][via]=rcvdpkt->mincost[i]+dt0.costs[via][via]) ){
			pkt0->mincost[i] = dt0.costs[i][via];
			change = 1;		/* A change has occurred. This will be useful a few lines down */
		}
	}

	printf("\nNew distance table is:\n");			/* Print the new distance table */
	printdt0(&dt0);

	if( change ){		/* Change has occurred! */ 
		printf("NODE0: Send new min event at t=%f\n\n", clocktime );
		/* send to all directly attached nodes (1, 2, 3) */
		for(i=0; i<4; i++ ){
			if(i==0 ) continue;
			pkt0->destid = i;
			tolayer2(*pkt0);		/* defined in prog3.c */
	}
	}
	else			/* Otherwise, nothing has ocurred in the min values! */
		printf("NODE0: No change in min values\n");

}

printdt0(dtptr)
  struct distance_table *dtptr;
  
{
  printf("                via     \n");
  printf("   D0 |    1     2    3 \n");
  printf("  ----|-----------------\n");
  printf("     1|  %3d   %3d   %3d\n",dtptr->costs[1][1],
	 dtptr->costs[1][2],dtptr->costs[1][3]);
  printf("dest 2|  %3d   %3d   %3d\n",dtptr->costs[2][1],
	 dtptr->costs[2][2],dtptr->costs[2][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][1],
	 dtptr->costs[3][2],dtptr->costs[3][3]);
}

linkhandler0(linkid, newcost)   
  int linkid, newcost;

/* called when cost from 0 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
	
{
}