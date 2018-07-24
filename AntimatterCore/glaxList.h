#pragma once
#include "SystemHeader.h"

//http://code.runnable.com/Us545Cg3skdWAAOo/how-to-clear-a-linked-list-remove-all-nodes-for-c%2B%2B

namespace Antimatter
{
	namespace glx
	{
		template<typename T>
		class List
		{
		private:
			typedef struct node
			{
				T data;
				node* next;

			}*nodeptr;	//decalre node* typedef

			nodeptr head;	  //should not change.  The  first element
			nodeptr curr;	  //used for sorting through the list
			nodeptr tmp;	  //tmp container for transfering/check elements
		public:
			List<T>()
			{
				head = NULL;
				curr = NULL;
				tmp = NULL;

			};
			List<T>(T)
			{
				head = NULL;
				curr = NULL;
				tmp = NULL;
				Add(T);
			}
			~List()	  //destructor
			{

			}

			int Length = 0;

			bool IsEmpty()
			{
				if (head == NULL)
					return true;

				return false;
			}

			void Add(T addData)
			{
				nodeptr n = new node();	  //create memorty for the new node
				n->next = NULL;			//the new node will contain yet contain a node after it
				n->data = addData;	   //store the data
				Length++;
				if (head != NULL)		//if the list is not empty
				{
					curr = head;  //start at the head
					while (curr->next != NULL) //while the node contains a following node
					{
						curr = curr->next;		//go to the following node
					}

					//we should have stopped in the last node of the chain.  Assign
					//the new node as the following node
					curr->next = n;
				}
				else //is the list if empty
				{
					head = n;	//set thwe value as the head
				}
			};

			void Remove(T deleteData)
			{
				nodeptr deletenode = NULL;
				tmp = head;		  //start at the head

				curr = head;
				Length--;
				//if we have reached the end and we haven't found the data we want to delete
				while (curr != NULL && curr->data != deleteData)
				{
					tmp = curr;		   //move to the next node and get the tmp to trail behind
					curr = curr->next;
				}

				//we have reached the end of the list
				if (curr == NULL)
				{
					cerr << deleteData << " was not found in list\n";
					delete deletenode;
				}
				else //found data in the node
				{
					//store the current as the node to be deleted
					deletenode = curr;
					//move to the next node
					curr = curr->next;
					//plug the gap.  Bind the node after the deltenode to the
					//trailing tmp node.  this removes deletenode from the chain
					tmp->next = curr;

					//the node the user wants to delete is the head node
					if (deletenode == head)
					{
						//set the next node as head node.  
						head = head->next;
						//reset tmp.  No trailing is needed
						tmp = NULL;
					}
					//delete the node
					delete deletenode;
					//cerr << "\n Value " << deleteData << " was deleted\n";
				}
			};

			void Clear()  //probably wrong
			{
				nodeptr *pDel = head;

				/* Traverse the list and delete the node one by one from the head */
				while (pDel != NULL) {
					/* take out the head node */
					head = head->next;
					delete pDel;
					/* update the head node */
					pDel = head;
				}
				/* Reset the head and tail node */
				tail = head = NULL;
				Length = 0;
			}

			//Array style accessor
			T operator[](int index)
			{

				//go the begining
				if (head == NULL)
				{
					cerr << "\n ->>List has no contents";
					//return NULL;
				}

				//TODO
				//value of index appears to be incorrect
				if (index > Count() - 1)
				{
					cerr << "\n - > > Index " << index << " not found in List";
					//return NULL;
				}
				curr = head;

				//loop through until we reach the index -1 (indexing starts at 0)
				for (int i = 0; i < index; i++)
				{
					curr = curr->next;
				}

				//return data
				return curr->data;
			}

			T* ToArray()
			{

				curr = head;

				if (curr == NULL)
				{
					cerr << "-> > List error current is NULL";
					return NULL;
				}
				else{
					//cerr << curr->data;
					T* arr = new T[Count()];
					List<T> locallist = *this;
					int i = 0;

					for (int i = 0; i < Count(); i++)
						arr[i] = locallist[i];

					return arr;
				}
			}

			int Count()
			{
				curr = head;	//start at the begining
				unsigned int count = 0;		//initialize count

				while (curr != NULL)	//check all that are not null
				{
					curr = curr->next;	//go to next
					count = count + 1;	 //increment count
				}
				return count;	   //return count.  If curr == NULL then no count occurs, we return 0;
			}

			bool Contains(T checkData)
			{
				curr = head;		  //start at the head
				while (curr != NULL)	  //loop through to the end
				{
					if (curr->data == checkData)  //check if the data in the 
						return true;			   //node is the data we all looking for
					//if successful break out the loop
					curr = curr->next;		  	   //go to next
				}

				//if we looped through and didn't break out of the loop (ie no data was found)
				//when return false;
				cerr << "\n Item " << checkData << " not found in Glaxion Natve List\n";
				return false;
			}


			//prnt yada yada
			void Print()
			{
				curr = head;
				while (curr != NULL)
				{
					cout << curr->data << "\n";
					curr = curr->next;
				}
			};

			void Print(char* text)
			{
				cerr << "\n Glaxion Native List: \n";
				cerr << "    " << text << "\n";
				Print();

			}
		};
	}
	
}