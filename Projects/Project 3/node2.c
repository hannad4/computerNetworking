// PLEASE NOTE: THE CODE FOR node2.c and node3.c ARE VERY SIMILAR TO node0.c and node1.c
// PLEASE REFER TO node0.c and node1.c FOR COMMENTS ON CODE

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
extern float clocktime;
struct rtpkt *pkt2;

struct distance_table 
{
  int costs[4][4];
} dt2;


/* students to write the following two routines, and maybe some others */

void rtinit2() 
{
	int i, j;
	pkt2 = (struct rtpkt *)malloc( sizeof(struct rtpkt) );
	pkt2->sourceid = 2;

	printf("NODE2: initialized at t=%f\n", clocktime );

	dt2.costs[2][0] = 3; dt2.costs[2][1] = 1; dt2.costs[2][2] = 0; dt2.costs[2][3] = 2;

	for (i=0; i<2; i++)
	{ 	
		for (j=0; j<4; j++)
		{ 
			dt2.costs[i][j]=999;
		}
	}
	for (i=3; i<4; i++)
	{ 	
		for (j=0; j<4; j++)
		{ 
			dt2.costs[i][j]=999;
		}
	}


	pkt2->mincost[0] = dt2.costs[0][0] = 3;	/* cost to node 0 is 3 */
	pkt2->mincost[1] = dt2.costs[1][1] = 1;	/* cost to node 1 is 1 */
	pkt2->mincost[2] = 0;					/* cost to itself is 0 */
	pkt2->mincost[3] = dt2.costs[3][3] = 2;	/* cost to node 3 is 2 */

	printf("Distance table is:\n");
	printdt2(&dt2);	

	pkt2->sourceid = 999;


	printf("NODE2: send new min event occurred at t=%f\n\n", clocktime );

	for(i=0; i<4; i++ ){
		if(i==2 ) continue;
		pkt2->destid = i;
		tolayer2(*pkt2);
	}
}


void rtupdate2(rcvdpkt)
  struct rtpkt *rcvdpkt;
  
{
	int i, change = 0, via = rcvdpkt->sourceid;
	
	printf("\nNODE2: Update occurred at t=%f\n", clocktime );

	for(i=0; i<4; i++ ){
		if(i==2 || i==via || rcvdpkt->mincost[i]==999) continue;

		if( pkt2->mincost[i] > (dt2.costs[i][via]=rcvdpkt->mincost[i]+dt2.costs[via][via]) ){
			pkt2->mincost[i] = dt2.costs[i][via];
			change = 1;
		}
	}

	printf("\nNew distance table is:\n");
	printdt2(&dt2);

	if( change ) {
		printf("NODE2: Send new min event at t=%f\n\n", clocktime );
		for(i=0; i<4; i++ ){
			if(i==2 ) continue;
			pkt2->destid = i;
			tolayer2(*pkt2);
		}
	}
	else
		printf("NODE2: No change in min values\n");
}

printdt2(dtptr)
  struct distance_table *dtptr;
  
{
  printf("                via     \n");
  printf("   D2 |    0     1    3 \n");
  printf("  ----|-----------------\n");
  printf("     0|  %3d   %3d   %3d\n",dtptr->costs[0][0],
	 dtptr->costs[0][1],dtptr->costs[0][3]);
  printf("dest 1|  %3d   %3d   %3d\n",dtptr->costs[1][0],
	 dtptr->costs[1][1],dtptr->costs[1][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][0],
	 dtptr->costs[3][1],dtptr->costs[3][3]);
}