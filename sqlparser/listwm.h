/** 
 * Copyright (c) 2016 SQLines
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// ListWM - Stores a list of pointers with multiple values (Wrapped list)

#ifndef sqlines_listwm_h
#define sqlines_listwm_h

class ListwmItem
{
public:
	void *value;
	void *value2;
	void *value3;
	void *value4;
	void *value5;
	void *value6;

    int ivalue;

	ListwmItem *prev;
	ListwmItem *next;

	ListwmItem()
	{
		value = NULL;
		value2 = NULL;
		value3 = NULL;
		value4 = NULL;
		value5 = NULL;
		value6 = NULL;

        ivalue = 0;

		prev = NULL;
		next = NULL;
	}
};

class ListWM
{
	// The first element in the list
	ListwmItem *first;
	// The last element in the list
	ListwmItem *last;

	// Total number of items in the list
	int count;

public:
	// Constructor
	ListWM()
	{
		first = NULL;
		last = NULL;
		count = 0;
	}

	// Destructor
	~ListWM()
	{
		DeleteAll();
	}

	// Delete all elements from the list
	void DeleteAll()
	{
		ListwmItem *current = first;
		ListwmItem *next = NULL;

		// remove list elements
		while(current)
		{
			next = current->next;
			delete current;
			current = next;
		}

		first = NULL;
		last = NULL;
		count = 0;
	}

	// Add a new item to the list
    void Add(void* value, void* value2 = NULL, void* value3 = NULL, void* value4 = NULL, void* value5 = NULL, void* value6 = NULL)
	{
        Add(value, value2, value3, value4, value5, value6, 0);
    }

    void Add(int ivalue)
	{
        Add(NULL, NULL, NULL, NULL, NULL, NULL, ivalue);
    }

	void Add(void* value, void* value2, void* value3, void* value4, void* value5, void* value6, int ivalue)
	{
		ListwmItem *item = new ListwmItem();

		item->value = value;
		item->value2 = value2;
		item->value3 = value3;
		item->value4 = value4;
		item->value5 = value5;
		item->value6 = value6;

        item->ivalue = ivalue;

		// check if it is the first item in the list
		if(!first)
		{
			first = item;
			item->prev = NULL;
		}
		else
		// otherwise link the previous element with the new and vice versa
		{
			last->next = item;
			item->prev = last;
		}

		last = item;
		item->next = NULL;

		// increase the number of items in the list
		count++;
	}

	// Delete the specified item
	void DeleteItem(ListwmItem *item)
	{
		if(item == NULL || count == 0)
			return;

		if(item == first)
			first = item->next;

		if(item->prev != NULL)
			item->prev->next = item->next;

		if(item == last)
			last = item->prev;

		if(item->next != NULL)
			item->next->prev = item->prev;

		delete item;

		count--;
	}

	// Delete items with specified pointers
	void DeleteItems(void* value, void* value2 = NULL, void* value3 = NULL, void* value4 = NULL, void* value5 = NULL)
	{
		ListwmItem *current = first;
		ListwmItem *next = NULL;

		// Remove list elements
		while(current)
		{
			next = current->next;

			bool del = true;

			// If input value is not set that means any item with this field removed if other value matched
			if((value != NULL && current->value != value) || (value2 != NULL && current->value2 != value2) ||
				(value3 != NULL && current->value3 != value3) || (value4 != NULL && current->value4 != value4) ||
				(value5 != NULL && current->value5 != value5))
				del = false;

			// Item is removed
			if(del == true)
			{
				if(current == first)
				{
					first = next;

					if(next != NULL)
						next->prev = NULL;
				}
				else
				if(current->prev != NULL)
					current->prev->next = next;
				
				// Both condition current == first and == last can be met simulteneously
				if(current == last)
				{
					last = current->prev;

					if(last != NULL)
						last->next = NULL;
				}
				else
				if(next != NULL)
					next->prev = current->prev;

				delete current;

				count--;
			}

			current = next;
		}
	}

	// Get the first value
	ListwmItem* GetFirst()
	{
		return first;
	}

	// Get the last value
	ListwmItem* GetLast()
	{
		return last;
	}

	// Get Nth value
	ListwmItem* GetNth(int n)
	{
		int k = 0;

		for(ListwmItem* i = first; i != NULL; i = i->next)
		{
			if(k == n)
				return i;

			k++;
		}		

		return NULL;
	}

	// Get the total number of items in the list
	int GetCount() 
	{ 
		return count; 
	}
};

#endif // sqlines_listwm_h
