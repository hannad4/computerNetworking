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
extern float clocktime;			/* The clock to be used for the emulator */
struct rtpkt *pkt1; 	/* This packet will hold the minimum distances */


struct distance_table 
{
  int costs[4][4];
} dt1;


/* students to write the following two routines, and maybe some others */

rtinit1() 
{
	int i, j;
	pkt1 = (struct rtpkt *)malloc( sizeof(struct rtpkt) );
	pkt1->sourceid = 1;

	printf("NODE1: initialized at t=%f\n", clocktime );
	
	dt1.costs[1][0] = 1; dt1.costs[1][1] = 0; dt1.costs[1][2] = 1; dt1.costs[1][3] = 999;  /* Defining the costs that are officially known */

	for (i=0; i<1; i++)
	{ 	
		for (j=0; j<4; j++)
		{ 
			dt1.costs[i][j]=999;		/* Setting costs to "infinity" which is 999 */
		}
	}

	for (i=2; i<4; i++)
	{ 	
		for (j=0; j<4; j++)
		{ 
			dt1.costs[i][j]=999;		/* Setting costs to "infinity" which is 999 */
		}
	}


	pkt1->mincost[0] = dt1.costs[0][0] = 1;	/* The cost to node 0 is 1 */
	pkt1->mincost[1] = 0;					/* The cost of a node to itself is 0 */
	pkt1->mincost[2] = dt1.costs[2][2] = 1; /* The cost to node 2 is 1 */
	pkt1->mincost[3] = 999;					/* A non directly attached node has a cost of infinity, or 999 */

	printf("Distance table is:\n");
	printdt1(&dt1);	/* print distance table */

	pkt1->sourceid = 1;		/* pkt1->sourceid is this node which is 1 */

	printf("NODE1: send new min event at t=%f\n\n", clocktime );
	
	/* send to all directly attached nodes (0, 2) */
	for(i=0; i<4; i++ ){
		if(i==1 || i==3 ) continue;
		pkt1->destid = i;
		tolayer2(*pkt1);			 /* Defined in prog3.c */
	}
}

rtupdate1(rcvdpkt)
  struct rtpkt *rcvdpkt;
  
{
	int i, change = 0, via = rcvdpkt->sourceid;
	
	printf("\nNODE1: Update occurred at t=%f\n", clocktime );

	for(i=0; i<4; i++ ){
		if(i==1 || i==via || rcvdpkt->mincost[i]==999) continue;

		if( pkt1->mincost[i] > (dt1.costs[i][via]=rcvdpkt->mincost[i]+dt1.costs[via][via]) ){
			pkt1->mincost[i] = dt1.costs[i][via];
			change = 1;		/* A change has occurred. This will be useful a few lines down */
		}
	}

	printf("\nNew distance table is:\n");
	printdt1(&dt1);

	if( change ){		/* Change has occurred! */
		printf("NODE1: Send new min event at t=%f\n\n", clocktime );
		for(i=0; i<4; i++ ){
			if(i==1 || i==3 ) continue;
			pkt1->destid = i;
			tolayer2(*pkt1);		/* Defined in prog3.c */
		}
	}
	else
		printf("NODE1: No change in min values\n");
}



printdt1(dtptr)
  struct distance_table *dtptr;
  
{
  printf("             via   \n");
  printf("   D1 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);
  printf("     3|  %3d   %3d\n",dtptr->costs[3][0], dtptr->costs[3][2]);

}


linkhandler1(linkid, newcost)   
int linkid, newcost;   
/* called when cost from 1 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
  
{
}