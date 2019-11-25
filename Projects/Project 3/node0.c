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
struct rtpkt *pkt0; 

struct distance_table 
{
  int costs[4][4];
} dt0;


/* students to write the following two routines, and maybe some others */

void rtinit0() 
{
  int i, j; 
  pkt0 = (struct rtpkt *)malloc( sizeof(struct rtpkt) );
	pkt0->sourceid = 0;

	printf("NODE0: initialization event at t=%f\n", clocktime );
	
	dt0.costs[0][0] = 0; dt0.costs[0][1] = 1; dt0.costs[0][2] = 3; dt0.costs[0][3] = 7; // initially known costs

	for (i=1; i<4; i++)
	{ 	
		for (j=0; j<4; j++)
		{ 
			dt0.costs[i][j]=999;
		}
	} 


	pkt0->mincost[0] = 0;					/* cost to itself (redundant => using 0) */
	pkt0->mincost[1] = dt0.costs[1][1] = 1;  /* cost to node 1 via 1 */
	pkt0->mincost[2] = dt0.costs[2][2] = 3;  /* cost to node 2 via 2 */
	pkt0->mincost[3] = dt0.costs[3][3] = 7;  /* cost to node 3 via 3 */

	printf("...after init distance table is:\n");
	printdt0(&dt0);	/* print conents of distance table after initialization */

	pkt0->sourceid = 0;

	sendMin0();		/* sends this node's current minimum (initialized) costs */
}


void rtupdate0(rcvdpkt)
  struct rtpkt *rcvdpkt;
{
  int i, change = 0, via = rcvdpkt->sourceid;
	
	printf("\nNODE0: rtupdate event at t=%f\n", clocktime );

	for(i=0; i<4; i++ ){
		if(i==0 || i==via || rcvdpkt->mincost[i]==999) continue;

		if( pkt0->mincost[i] > (dt0.costs[i][via]=rcvdpkt->mincost[i]+dt0.costs[via][via]) ){
			pkt0->mincost[i] = dt0.costs[i][via];
			change = 1;
		}
	}

	printf("...after update distance table is:\n");
	printdt0(&dt0);

	if( change )
		sendMin0();
	else
		printf("NODE0: min values haven't been changed\n");

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
