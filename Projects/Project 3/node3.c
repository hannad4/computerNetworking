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
struct rtpkt *pkt3;


struct distance_table 
{
  int costs[4][4];
} dt3;

/* students to write the following two routines, and maybe some others */

void rtinit3() 
{
	int i, j;
	pkt3 = (struct rtpkt *)malloc( sizeof(struct rtpkt) );
	pkt3->sourceid = 3;

	printf("NODE3: initialized at t=%f\n", clocktime );

	dt3.costs[3][0] = 7; dt3.costs[3][1] = 999; dt3.costs[3][2] = 2; dt3.costs[3][3] = 0;
	
	for (i=0; i<3; i++)
	{ 	
		for (j=0; j<4; j++)
		{ 
			dt3.costs[i][j]=999;
		}
	}

	pkt3->mincost[0] = dt3.costs[0][0] = 7;	/* cost to node 0 is 7 */
	pkt3->mincost[1] = 999;					/* cost to non-directly attached node is infinity, or 999 */
	pkt3->mincost[2] = dt3.costs[2][2] = 2;	/* cost to node 3 is 2 */
	pkt3->mincost[3] = 0;					/* cost to itself is 0 */

	printf("Distance table is:\n");
	printdt3(&dt3);	

	pkt3->sourceid = 3;

	printf("NODE3: send new min event at t=%f\n\n", clocktime );

	for(i=0; i<4; i++ ){
		if(i==3 || i==1 ) continue;
		pkt3->destid = i;
		tolayer2(*pkt3);
	}
}

void rtupdate3(rcvdpkt)
  struct rtpkt *rcvdpkt;
{
	int i, change = 0, via = rcvdpkt->sourceid;
	
	printf("\nNODE3: Update occurred at t=%f\n", clocktime );

	for(i=0; i<4; i++ ){
		if(i==3 || i==via || rcvdpkt->mincost[i]==999) continue;

		if( pkt3->mincost[i] > (dt3.costs[i][via]=rcvdpkt->mincost[i]+dt3.costs[via][via]) ){
			pkt3->mincost[i] = dt3.costs[i][via];
			change = 1;
		}
	}

	printf("\nNew distance table is:\n");
	printdt3(&dt3);

	if( change ) {
		printf("NODE3: send new min event at t=%f\n\n", clocktime );
		for(i=0; i<4; i++ ){
			if(i==3 || i==1 ) continue;
			pkt3->destid = i;
			tolayer2(*pkt3);
		}
	}
	else
		printf("NODE3: No change in min values\n");
}

printdt3(dtptr)
  struct distance_table *dtptr;
  
{
  printf("             via     \n");
  printf("   D3 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 1|  %3d   %3d\n",dtptr->costs[1][0], dtptr->costs[1][2]);
  printf("     2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);

}