/**********************************************************************
 * Copyright (c) 2018
 *	Sang-Hoon Kim <sanghoonkim@ajou.ac.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTIABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 **********************************************************************/
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "config.h"


/**
 * Skeleton data structures to implement the buddy system allocator
 */

/**
 * Data structure to represent an order-@order pages. To the rest of this file,
 * consecutive pages will be represented in @start:@order notation.
 * E.g., 16:3 is 8(2^3)  consecutive pages (or say order-3 page) starting from
 * page frame 16.
 */
struct chunk {
	/**
	 * TODO: Modify this structure as you need.
	 */
	struct chunk *next;
	unsigned int start;
	unsigned int order;
};


/**
 * Data structure to maintain order-@order free chunks.
 * NOTE that chunk_list SHOULD WORK LIKE THE QUEUE; the firstly added chunk
 * should be used first, otherwise the grading system will fail.
 */
struct chunk_list {
	/**
	 * TODO: Modify this structure as you need
	 */
	struct chunk *head;
	struct chunk *tail;
	unsigned int order;
};


/**
 * Data structure to realize the buddy system allocator
 */
struct buddy {
	/**
	 * TODO: Modify this example data structure as you need
	 */

	/**
	 * Free chunk list in the buddy system allocator.
	 *
	 * @NR_ORDERS is @MAX_ORDER + 1 (considering order-0 pages) and deifned in
	 * config.h. @MAX_ORDER is set in the Makefile. MAKE SURE your buddy
	 * implementation can handle order-0 to order-@MAX_ORDER pages.
	 */
	struct chunk_list chunks[NR_ORDERS];

	unsigned int allocated;	/* Number of pages that are allocated */
	unsigned int free;		/* Number of pages that are free */
};


/**
 * This is your buddy system allocator instance!
 */
static struct buddy buddy;


/**
 *    Your buddy system allocator should manage from order-0 to
 *  order-@MAX_ORDER. In the following example, assume your buddy system
 *  manages page 0 to 0x1F (0 -- 31, thus @nr_pages is 32) and pages from
 *  20 to 23 and 28 (0x14 - 0x17, 0x1C) are allocated by alloc_pages()
 *  by some orders.
 *  At this moment, the buddy system will split the address space into;
 *
 *      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
 * 0x00 <-------------------4-------------------------->
 * 0x10 <----2----->X  X  X  X  <-----2---->X  <0><-1-->
 *                  ^  ^  ^  ^              ^
 *                  |--|--|--|--------------|
 *                     allocated
 *
 *   Thus, the buddy system will maintain the free chunk lists like;
 *
 * Order     | Start addresses of free chunks
 * ----------------------------------------------
 * MAX_ORDER |
 *    ...    |
 *     4     | 0x00
 *     3     |
 *     2     | 0x10, 0x18
 *     1     | 0x1e
 *     0     | 0x1d
 */


/**
 * Allocate 2^@order contiguous pages.
 *
 * Description:
 *    For example, when @order=0, allocate a single page, @order=2 means
 *  to allocate 4 consecutive pages, and so forth.
 *    From the example state above, alloc_pages(2) gives 0x10 through @*page
 *  and the corresponding entry is removed from the free chunk. NOTE THAT the
 *  free chunk lists should be maintained as 'FIFO' so alloc_pages(2) returns 
 *  0x10, not 0x18. 
 *    To hanle alloc_pages(3), the order-4 chunk (0x00 -- 0x0f) should
 *  be broken into smaller chunks (say 0x00 -- 0x07, 0x08 -- 0x0f), and
 *  the LEFT BUDDY will be returned through @page whereas RIGHT BUDDY
 *  will be put into the order-3 free chunk list.
 *
 * Return:
 *   0      : On successful allocation. @*page will contain the starting
 *            page number of the allocated chunk
 *  -EINVAL : When @order < 0 or @order > MAX_ORDER
 *  -ENOMEM : When order-@order contiguous chunk is not available in the system
 */
int check_nextpage_exist(int order)
{
	int next_order = order + 1;
	if(buddy.chunks[next_order].head == NULL){
		//fprintf(stderr, "there is no node on %d\n", next_order);
		return 0;
	}
	else {	
		return next_order;
	}
}

int check_page_exist(int order)
{
	/*
	0 -> empty 
	1 -> one node
	2 -> more than one node
	*/
	if(buddy.chunks[order].head == NULL){
		return 0;
	}
	else{
		if(buddy.chunks[order].head->next == NULL){
			return 1;
		}
		else{
			return 2;
		}
	}
}

unsigned int return_node(int order)
{
	unsigned int ret;
	if(buddy.chunks[order].head->next == NULL){
		ret = buddy.chunks[order].head->start;
		buddy.chunks[order].head = NULL;
		free(buddy.chunks[order].head);
		return ret;
	}
	else{
		struct chunk *head_temp = buddy.chunks[order].head;
		ret=buddy.chunks[order].head->start;
		buddy.chunks[order].head = buddy.chunks[order].head->next;
		free(head_temp);
		return ret; 
	}
}

int alloc_pages(unsigned int *page, const unsigned int order)
{
		
	/*	
	int param_order = order;
	int check = check_page_exist(param_order);
	
	//fprintf(stderr, "check is: %d\n", check);
	unsigned int exi_add=0;
	
	if(buddy.free == 0){
		return -ENOMEM;		
	}	

	if(check == 0){
		// list has no node
		int i = 0;
		int up_page;
		unsigned int up_page_start;
		unsigned int gen_add = 0;

		//param_order = param_order+1;
		
		for(i=param_order; i<NR_ORDERS; i++)
		{
			Loop:
			up_page = check_nextpage_exist(param_order);
			//fprintf(stderr, "up page is :%d\n", up_page);
			if(up_page == 0){
				param_order = param_order + 1;
				goto Loop;
			}else{
				break;
			}
		}
		
		for(i=up_page; i>order; i--)
		{
			//fprintf(stderr, "i:%d\n", i);
			up_page_start = buddy.chunks[i].head->start;
			gen_add = return_node(i);
			
			struct chunk *node1 = (struct chunk *)malloc(sizeof(struct chunk)*1);
			node1->start = gen_add;
			node1->order = i-1;

			struct chunk *node2 = (struct chunk *)malloc(sizeof(struct chunk)*1);
			node2->start = gen_add + (1 << (i-1));
			node2->order = i-1;

			node1->next = node2;
			node2->next = NULL;
			buddy.chunks[i-1].head = node1;
			buddy.chunks[i-1].tail = node2;

		//	fprintf(stderr, "node1 address:%d\n", buddy.chunks[i-1].head->start);
		//	fprintf(stderr, "node2 address:%d\n", buddy.chunks[i-1].head->next->start);

			unsigned int temp = return_node(i-1);
		}
		*page = gen_add;
	//	fprintf(stderr ,"1:%d\n", buddy.chunks[i].head->start);
	//	fprintf(stderr, "out i :%d\n", i);
	//	unsigned int temp = return_node(i);
		buddy.allocated += (1 << order);
		buddy.free -= (1 << order);
		return 0;

	}
	else{
		// list has more than one node
		exi_add = return_node(param_order);
		*page = exi_add;
		buddy.allocated += (1 << order);
		buddy.free -= (1 << order);
		return 0;
	 
	}
*/
	
	/**
	 * Your implementation will look (but not limited to) like;
	 *
	 * Check whether a chunk is available from chunk_list of @order
	 * if (exist) {
	 *    allocate the chunk from the list; Done!
	 * } else {
	 *    Make an order-@order chunk by breaking a higher-order chunk(s)
	 *    - Find the smallest free chunk that can satisfy the request
	 *    - Break the LEFT chunk until it is small enough
	 *    - Put remainders into the free chunk list
	 *
	 *    Return the allocated chunk via @*page
	 * }
	 *
	 *----------------------------------------------------------------------
	 * Print out below message using PRINTF upon each events. Note it is
	 * possible for multiple events to be happened to handle a single
	 * alloc_pages(). Also, MAKE SURE TO USE 'PRINTF', _NOT_ printf, otherwise
	 * the grading procedure will fail.
	 *
	 * - Split an order-@x chunk starting from @page into @left and @right:
	 *   PRINTF("SPLIT 0x%x:%u -> 0x%x:%u + 0x%x:%u\n",
	 *			page, x, left, x-1, right, x-1);
	 *
	 * - Put an order-@x chunk starting from @page into the free list:
	 *   PRINTF("PUT   0x%x:%u\n", page, x);
	 *
	 * - Allocate an order-@x chunk starting from @page for serving the request:
	 *   PRINTF("ALLOC 0x%x:%x\n", page, x);
	 *
	 * Example: A order-4 chunk starting from 0 is split into 0:3 and 8:3,
	 * and 0:3 is split again to 0:2 and 4:2 to serve an order-2 allocation.
	 * And then 0:2 is allocated:
	 *
	 * SPLIT 0x0:4 -> 0x0:3 + 0x8:3
	 * PUT   0x8:3
	 * SPLIT 0x0:3 -> 0x0:2 + 0x4:2
	 * PUT   0x4:2
	 * ALLOC 0x0:2
	 *
	 *       OR
	 *
	 * SPLIT 0x0:4 -> 0x0:3 + 0x8:3
	 * SPLIT 0x0:3 -> 0x0:2 + 0x4:2
	 * PUT   0x8:3
	 * PUT   0x4:2
	 * ALLOC 0x0:2
	 *
	 *       OR
	 *
	 * SPLIT 0x0:4 -> 0x0:3 + 0x8:3
	 * SPLIT 0x0:3 -> 0x0:2 + 0x4:2
	 * PUT   0x4:2
	 * PUT   0x8:3
	 * ALLOC 0x0:2
	 *----------------------------------------------------------------------
	 */

//	buddy.allocated += (1 << order);
//	buddy.free -= (1 << order);
//	return -ENOMEM;
	int i;
//	int up_page;

	if(buddy.free <= 0)
	{
			return -ENOMEM;
	}
	if(order > 11)
	{
			return -EINVAL;
	}

	if(buddy.chunks[order].head != NULL)
	{
		struct chunk *temp;
		*page = buddy.chunks[order].head->start;
		temp = buddy.chunks[order].head;
		//correction
		if(temp->next == NULL)
			buddy.chunks[order].head = NULL;
		else
			buddy.chunks[order].head = buddy.chunks[order].head->next;		
		free(temp);
		buddy.allocated += (1 << order);
		buddy.free -= (1 << order);
		return 0;
	}
	else
	{
		for(i = order; i < NR_ORDERS; i++)
		{
			if(buddy.chunks[i].head == NULL)
			{
					continue;
			}
			else
			{
					int j;
					for(j = i; j > order; j--)
        			{
            			struct chunk *node1 = (struct chunk *)malloc(sizeof(struct chunk)*1);
       				    node1->start = buddy.chunks[j].head->start;
         				node1->order = j-1;
	
    			        struct chunk *node2 = (struct chunk *)malloc(sizeof(struct chunk)*1);
           			 	node2->start = buddy.chunks[j].head->start + (1 << (j-1));
         			    node2->order = j-1;
			
          			    node1->next = node2;
						node2->next = NULL;
           	
           			 	buddy.chunks[j-1].head = node1;
           			// 	buddy.chunks[j-1].tail = node2;
						struct chunk *temp2;
						temp2 = buddy.chunks[j].head;
						buddy.chunks[j].head = buddy.chunks[j].head->next;
           			 	free(temp2);
       				 }
      				 *page = buddy.chunks[j].head->start;
					 struct chunk *temp;
					 temp = buddy.chunks[j].head;
					 buddy.chunks[j].head = buddy.chunks[j].head->next;
					 free(temp);
       				 buddy.allocated += (1 << order);
      				 buddy.free -= (1 << order);
       				 return 0;
		   }
		}
	}
	
}


/**
 * Free @page which are contiguous for 2^@order pages
 *
 * Description
 *    Assume @page was allocated by alloc_pages(@order) above. 
 *  WARNING: When handling free chunks, put them into the free chunk list
 *  carefully so that free chunk lists work in FIFO.
 */
void print_node(unsigned int order)
{
	int i = order;
	int count=0;
	struct chunk *temp;
	for(i; i<NR_ORDERS; i++)
	{
//		fprintf(stderr, "order is :%d, head adress is:%d", order, buddy.chunks[order].head->start);
		temp = buddy.chunks[order].head;
		count++;
		while(temp != NULL)
		{
			temp = temp->next;
			count++;
		}
//		fprintf(stderr, "order node number is:%d", count);
		count = 9;
	}
}

int left_right(unsigned int page, const unsigned int order)
{
	int mod = (1 << (order+1));
	int result = page % mod;
	if(result == 0)
		return 0; //left side
	else 
		return 1; //right side 
}

int check_pair(unsigned int page, const unsigned int order, int flag)
{
	// return 0 : there is no pair, just add
	// return 1 : there is pair
	unsigned int my_addr = page;
	unsigned int pair_addr;
	struct chunk *temp;
	if(flag == 0)
	{
		// flag left 
		pair_addr = my_addr + (1 << order);
		temp = buddy.chunks[order].head;
		while(temp != NULL)
		{
			if(temp->start == pair_addr){
					return 1;
					break ;
			}
			else{
				temp = temp->next;
				continue;
			}
		}
		return 0;	
	}
	else
	{		
		// flag right
		pair_addr = my_addr - (1 << order);
		temp = buddy.chunks[order].head;
		while(temp != NULL)
		{
			if(temp->start == pair_addr){
					return 1;
					break;
			}
			else{
				temp = temp->next;
				continue;
			}
		}
		return 0;
	}
}

int add_node(unsigned int page, const unsigned int order, int head_flag)
{
	//fprintf(stderr, "****************************ADD PROCESS****************************\n");
	struct chunk *node = (struct chunk *)malloc(sizeof(struct chunk)*1);
	if(head_flag == 1)
	{
		//노드를 헤드에다가 추가
		node->start = page;
		node->order = order;
		buddy.chunks[order].head = node;
		buddy.chunks[order].head->next = NULL;
	}
	else
	{
		//노드를 꼬리에다가 추가
		struct chunk *tail;
		tail = buddy.chunks[order].head;
		while(tail->next != NULL)
		{
			tail = tail->next;
		}
		node->start = page;
		node->order = order;
		tail->next = node;
		node->next = NULL;
	}
	
}

unsigned int free_node(unsigned int addr, int start_point, int end_point, int flag)
{
    int start = start_point;
    int end = end_point;
    int lr_flag;
    unsigned int rule_addr=addr; 
    unsigned int free_addr;
    struct chunk *before;
    struct chunk *current;
    for(int i= start; i<=end; i++){ 
        lr_flag = left_right(rule_addr, i);
        if(lr_flag == 0){
            free_addr = rule_addr + (1 << i);
        }else{
            free_addr = rule_addr - (1 << i);
            rule_addr = free_addr;
        }
        current = buddy.chunks[i].head;
        if(current == NULL){
            // no node 
                buddy.chunks[i].head = NULL;
        }
        else if(current->next == NULL){
            // only head node
            if(free_addr == current->start){
                    free(current);
                    buddy.chunks[i].head = NULL;
            }else{
            }
        }
        else{
            // more than one node
            if(free_addr == current->start){
                //head is free address
                buddy.chunks[i].head = current->next;
                free(current);
            }
            else{
                //head is not free address
                before = current; current = current->next;
                while(current != NULL){
                    if(free_addr == before->next->start){
                            if(current->next != NULL){
                                    before->next = current->next;
                                    free(current);
									break;
                            }else{
                                before->next = NULL;
                                free(current);
								break;
                            } 
					}
					before = current; current = current->next;
				}
			}
		}
	}
	return rule_addr;
}  
/*
void free_node(unsigned int addr, unsigned int order)
{
	unsigned int target_addr = addr;
	unsigned int target_order = order;
	
	struct chunk* before;
	struct chunk* current = buddy.chunks[target_order].head;



	if(current == NULL)
	{
		fprintf(stderr, "11111111111111111\n");
		//there is no node
		buddy.chunks[target_order].head = NULL;
	}
	else if(current->next == NULL)
	{
		fprintf(stderr, "2222222222222222\n");
		fprintf(stderr, " target addr:%d\n", target_addr);
		fprintf(stderr, " current start:%d\n", current->start);
		//there is just head node 
		if(target_addr == current->start){
			buddy.chunks[target_order].head = NULL;
		}
		else{ 
			return;
		}
	}
	else
	{
		//there is more than one node
		fprintf(stderr, "333333333333333\n");
		fprintf(stderr, " target addr:%d\n", target_addr);
		fprintf(stderr, " current start:%d\n", current->start);
		fprintf(stderr, "there is more than one node\n");
		before = current;
		if(target_addr == current->start)
		{
			if(current->next != NULL){
				struct chunk *temp;
				temp=current;
				buddy.chunks[target_order].head = current->next;
				free(temp);
			}
			else{
				free(current);
				buddy.chunks[target_order].head = NULL;
			}
		}
		current = current->next;
		while(current != NULL){
			fprintf(stderr, "current is %d\n", before->next->start);
			if(target_addr == before->next->start){
				if(current->next != NULL){
						before->next = current->next;
						free(current);
				}else{
					before->next = NULL;
					free(current);
				}
			}
			before = current;
			current = current->next;
		}
	}	
}
*/
int merge(unsigned int page, const unsigned int order)
{
//	fprintf(stderr,"************************MERGE PROCESS**************************\n");
	int i = order;
	unsigned int addr;
	unsigned int free_order = order-1;
//	unsigned int free_addr;
	struct chunk *node = (struct chunk *)malloc(sizeof(struct chunk)*1);
//	int left_right_flag;
//	int pair_exist_flag;
		

	addr = page;
	if(buddy.chunks[i].head == NULL)
	{
		node->start = addr;
		node->order = i;	
		node->next  = NULL; 
		buddy.chunks[i].head = node;
	}
	else
	{
		struct chunk *tail;
		tail = buddy.chunks[i].head;
		while(tail->next != NULL)
		{
				tail = tail->next; 
		}
		node->start = addr;
		node->order = i;
		tail->next = node;
		node->next = NULL;
    }
}

void free_pages(unsigned int page, const unsigned int order)
{
    int left_right_flag;
    int pair_exist_flag;
	int temp=order;
	int end=0;
    unsigned int my_addr=page;
    unsigned int free_addr = page;
    
    left_right_flag = left_right(page,order);
    pair_exist_flag = check_pair(page,order,left_right_flag);
    
    if(pair_exist_flag == 0)
    {
        // 내 pair 가 존재하지 않을 경우
        int head_flag;
        if(buddy.chunks[order].head == NULL)
            head_flag = 1; // 아무노드도 존재 안함
        else 
            head_flag = 0; // 하나이상의 노드 존재
    
        add_node(my_addr, order, head_flag);
		buddy.allocated -= (1 << order);
		buddy.free += (1 << order);
		return;
    }
    else
    {
        // 내 pair가 존재할 경우
        int start=order; 
		 while(pair_exist_flag != 0 && temp<11){                              
        	if(left_right_flag == 0)
       	    	 my_addr = my_addr; //  내 주소가 왼쪽 주소일 경우 
        	else 
       	    	 my_addr = my_addr - (1 << temp); // 내 주소가 오른쪽 주소일 경우
			
			temp = temp+1;
            left_right_flag = left_right(my_addr, temp);
            pair_exist_flag = check_pair(my_addr, temp, left_right_flag);
        }
        end = temp-1;
        unsigned int target_addr = free_node(free_addr, order, end,left_right_flag);
        merge(target_addr, temp);
		buddy.allocated -= (1 << order);
		buddy.free += (1 << order);
			
    }
	return;
}

/*
void free_pages(unsigned int page, const unsigned int order)
{
	
	 * Your implementation will look (but not limited to) like;
	 *
	 * Find the buddy chunk from this @order.
	 
	 * if (buddy does not exist in this order-@order free list) {
	 *    put into the TAIL of this chunk list. Problem solved!!!
	 * } else {
	 *    Merge with the buddy
	 *    Promote the merged chunk into the higher-order chunk list
	 *
	 *    Consider the cascading case as well; in the higher-order list, there
	 *    might exist its buddy again, and again, again, ....
	 * }
	 *
	 *----------------------------------------------------------------------
	 * Similar to alloc_pages() above, print following messages using PRINTF
	 * when the event happens;
	 *
	 * - Merge order-$x buddies starting from $left and $right:
	 *   PRINTF("MERGE : 0x%x:%u + 0x%x:%u -> 0x%x:%u\n",
	 *			left, x, right, x, left, x+1);
	 *
	 * - Put an order-@x chunk starting from @page into the free list:
	 *   PRINTF("PUT  : 0x%x:%u\n", page, x);
	 *
	 * Example: Two buddies 0:2 and 4:2 (0:2 indicates an order-2 chunk
	 * starting from 0) are merged to 0:3, and it is merged again with 8:3,
	 * producing 0:4. And then finally the chunk is put into the order-4 free
	 * chunk list:
	 *
	 * MERGE : 0x0:2 + 0x4:2 -> 0x0:3
	 * MERGE : 0x0:3 + 0x8:3 -> 0x0:4
	 * PUT   : 0x0:4
	 *----------------------------------------------------------------------

	int left_right_flag;
	int pair_exist_flag;
	int head_flag;
	unsigned int my_addr;
	unsigned int pair_addr;
	struct chunk *tail_node;
	struct chunk *tail_before_node;

	left_right_flag = left_right(page, order);
	pair_exist_flag = check_pair(page, order, left_right_flag);
	fprintf(stderr, "if there is pair 1, no pair 0 : %d\n", pair_exist_flag);
	if(left_right_flag == 0){
		// my node is left node
		fprintf(stderr, "my node is left node...\n");
		int temp;
		int recursive;
		my_addr = page;
		pair_addr = page + (1 << order);
		if(pair_exist_flag == 1){
			// there is pair, merge
			fprintf(stderr, "i have a pair! just merge me\n");
			merge(my_addr, order+1,0);
			//fprintf(stderr,"asdfasdf:%d\n", buddy.chunks[order].head->start);
			temp = left_right(my_addr, order+1);
			recursive = check_pair(my_addr, order+1, temp);
			tail_node = buddy.chunks[order+1].head;
			if(recursive == 1){
				if(order+1 != 11){
					fprintf(stderr, "my addr:%d\n", my_addr);
					fprintf(stderr, "here is recursive problem \n");
					tail_node = buddy.chunks[order+1].head; 
					while(tail_node != NULL)
					{
						if(tail_node->next->start == my_addr){
							tail_before_node = tail_node;
							tail_node = tail_node->next;
						}
						tail_node = tail_node->next;
					}
					if(tail_before_node != NULL && tail_node != NULL){
							tail_before_node->next = NULL;
							free(tail_node);
					}
					free_pages(my_addr, order+1);  //once more merge
				}
			}
			else{
				return;
			}
		}
		else{
			// there is no pair, just add
			fprintf(stderr, "i dont have a pair! just add me\n");
			if(buddy.chunks[order].head == NULL){
				head_flag = 1;
				add_node(my_addr, order, head_flag);
			}
			else{
				head_flag = 0;
				add_node(my_addr, order, head_flag);
			}
		}
	}
	else{
		// my node is right node
		fprintf(stderr, "my node is right node...\n");
		int temp;
		int recursive;
		my_addr = page;
		pair_addr = page - (1 << order);
		if(pair_exist_flag == 1){
			fprintf(stderr, "i have a pair! just merge me\n");
			merge(pair_addr, order+1,1);
			temp = left_right(pair_addr, order+1);
			recursive = check_pair(pair_addr, order+1, temp);
			if(recursive == 1){
				tail_node = buddy.chunks[order+1].head;
				if(order+1 != 11){
                    while(tail_node != NULL)
                    {                                                                                                                                                                          
                        if(tail_node->next->start == pair_addr){
                            tail_before_node = tail_node;
                            tail_node = tail_node->next;
                        }
                        tail_node = tail_node->next;
                    }
                    if(tail_before_node != NULL && tail_node != NULL){
                            tail_before_node->next = NULL;
                            free(tail_node);
                    }
				}
				free_pages(pair_addr, order+1);
			}
			else{ 
				return;
			}
		}
		else{
			fprintf(stderr,"i dont have a pair! just add me\n");
			if(buddy.chunks[order].head == NULL){
                head_flag = 1;
                add_node(my_addr, order, head_flag);
            }
            else{
                head_flag = 0;
                add_node(my_addr, order, head_flag);
            }
		}
	}
	
	buddy.allocated -= (1 << order);
	buddy.free += (1 << order);
}
*/

/**
 * Print out the order-@order free chunk list
 *
 *  In the example above, print_free_pages(0) will print out:
 *  0x1d:0
 *
 *  print_free_pages(2):
 *    0x10:2
 *    0x18:2
 */
void print_free_pages(const unsigned int order)
{
	//unsigned int starting_page = 0x43; /* I love 43 because it's perfect!! */
	struct chunk *node = buddy.chunks[order].head;
	while(node != NULL)
	{	
		fprintf(stderr, "	0x%x:%u\n", node->start, node->order); 
		node = node->next;
	}
	/**
	 * Your implementation should print out each free chunk from the beginning
	 * in the following format.
	 * WARNING: USE fprintf(stderr) NOT printf, otherwise the grading
	 * system will evaluate your implementation wrong.
	 */
}


/**
 * Return the unusable index(UI) of order-@order.
 *
 * Description:
 *    Return the unusable index of @order. In the above example, we have 27 free
 *  pages;
 *  # of free pages =
 *    sum(i = 0 to @MAX_ORDER){ (1 << i) * # of order-i free chunks }
 *
 *    and
 *
 *  UI(0) = 0 / 27 = 0.0 (UI of 0 is always 0 in fact).
 *  UI(1) = 1 (for 0x1d) / 27 = 0.037
 *  UI(2) = (1 (0x1d) + 2 (0x1e-0x1f)) / 27 = 0.111
 *  UI(3) = (1 (0x1d) + 2 (0x1e-0x1f) + 4 (0x10-0x13) + 4 (0x18-0x1b)) / 27
 *        = 0.407
 *  ...
 */
double get_unusable_index(unsigned int order)
{
	double use = 0;
	struct chunk *temp;
	for(int i=order-1; i>-1; i--)
	{
		temp = buddy.chunks[i].head;
		while(temp != NULL)
		{
				use = use + (1 << temp->order);
				temp = temp->next;
		}
	}
	use = use/(double)buddy.free;
	return use;
}


/**
 * Initialize your buddy system.
 *
 * @nr_pages_in_order: number of pages in order-n notation to manage.
 * For instance, if @nr_pages_in_order = 13, the system should be able to
 * manage 8192 pages. You can set @nr_pages_in_order by using -n option while
 * launching the program;
 * ./pa4 -n 13       <-- will initiate the system with 2^13 pages.
 *
 * Return:
 *   0      : On successful initialization
 *  -EINVAL : Invalid arguments or when something goes wrong
 */
int init_buddy(unsigned int nr_pages_in_order)
{
	int i;
	buddy.allocated = 0;
	buddy.free = 1 << nr_pages_in_order;
	struct chunk* node1;
	struct chunk* node0;
	/* TODO: Do your initialization as you need */

	for (i = 0; i < NR_ORDERS; i++) {
		buddy.chunks[i].order = i;
	}
	int total = (1 <<(nr_pages_in_order - 11));
	struct chunk* temp;
	if(nr_pages_in_order > 11)
	{
		node0 = (struct chunk*)malloc(sizeof(struct chunk)*1);
        node0->start = 0;
        node0->order = 11;
        node0->next = NULL;
  		buddy.chunks[11].head = node0;
		
		temp = buddy.chunks[11].head;
		for(i = 1; i < total; i++)
		{
			node1 = (struct chunk*)malloc(sizeof(struct chunk)*1);
			node1->start = i*(1 << 11);
			node1->order = 11;
			node1->next = NULL;
			
			temp->next = node1;
			temp = temp->next;
		}
	}
	/*
	//psize = nr_pages_in_order;
	struct chunk *node1 = (struct chunk *)malloc(sizeof(struct chunk)*1);
	node1->start = 0;
	//node1->order = psize -1;
	node1->order = nr_pages_in_order - 1;

	struct chunk *node2 = (struct chunk *)malloc(sizeof(struct chunk)*1); 
	node2->start = 1 << (nr_pages_in_order - 1);
	//node2->order = psize -1;
	node2->order = nr_pages_in_order - 1;
	
	node1->next = node2;
	node2->next = NULL;

	buddy.chunks[NR_ORDERS -1].head = node1;
	buddy.chunks[NR_ORDERS -1].tail = node2;
	*/
	/**
	 * TODO: Don't forget to initiate the free chunk list with
	 * order-@MAX_ORDER chunks. Note you might add multiple chunks if
	 * @nr_pages_in_order > @MAX_ORDER. For instance, when
	 * @nr_pages_in_order = 10 and @MAX_ORDER = 9, the initial free chunk
	 * lists will have two chunks; 0x0:9, 0x200:9.
	 */

	return 0;
}


/**
 * Return resources that your buddy system has been allocated. No other
 * function will be called after calling this function.
 */
void fini_buddy(void)
{	
	/**
	 * TODO: Do your finalization if needed, and don't forget to release
	 * the initial chunks that you put in init_buddy().
	 */

	 for(int i=0; i<NR_ORDERS; i++)
	 {
			if(buddy.chunks[i].head == NULL){
				continue;
			}
			else{
			//buddy.chunks[i].head != NULL		 
			 	struct chunk *current = buddy.chunks[i].head;
				struct chunk *temp = current;
				while(current != NULL)
				{
					temp = current;
					current = current->next;
					free(temp);
				}
	//			free(temp);

	 		}
	 }
	 
}


