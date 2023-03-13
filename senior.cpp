#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <array>
#include <string>
#include <sstream>
#include <fstream>
#include <iterator>

#include "resource.h"

using namespace std;

struct food_record
{
	string id;
	string name;
	double price;
	vector <string> component;
	
	double component_sum;
	double discount;
};

map <string, struct food_record> m_fr;

istream &operator >> (istream &is , vector <string> &v)
{
	while (is)
	{
		string s;
		
		getline(is,s,',');

		if (s.size())
			v.push_back(s);
		else
			break;
	}
	
	return is;
}

istream &operator >> (istream &is , struct food_record &r)
{
	string s;
	
	getline(is,s,',');
	r.id = s;
	
	getline(is,s,',');
	r.name = s;
	
	getline(is,s,',');
	r.price = atof(s.c_str());
	
	s.clear();
	getline(is,s);

	istringstream iss(s);
	
	iss >> r.component;
	
	sort ( r.component.begin() , r.component.end() , [](const string &l, const string &r){ return l > r; });
	return is;
}

ostream &operator << (ostream &os, const struct food_record &r)
{
	os << r.id << "," << r.name << "," << r.price;
	if (r.component.size())
	{
		for ( auto i : r.component )
		{
			os << "," << i;
		}
	}
	os << ":" << r.component_sum << ":" << r.discount;
	return os;
}

vector <string> sorted_menu;

void init_menu()
{
	ifstream ifs("menu.csv");
	
	while (ifs)
	{
		struct food_record r;
		
		string s;
		getline(ifs,s);
		
		if (s.size())
		{
			istringstream iss(s);
			iss >> r;
			if (ifs)
			{
				r.component_sum = 0;
				m_fr[r.id]=r;
			}
		}
	}
	
	for (map <string, struct food_record>::iterator o =  m_fr.begin(); o != m_fr.end(); o++)
	{
		struct food_record &r = o->second;
		r.component_sum =0;
		for (auto i: r.component)
		{
			r.component_sum += m_fr[i].price;
		}
		r.discount = r.component_sum - r.price;
			
		
		sorted_menu.push_back(r.id);
	}
	
	
	sort(sorted_menu.begin(),sorted_menu.end(),[](const string &lid,const string &rid)
	{
		const struct food_record &lo = m_fr[lid];
		const struct food_record &ro = m_fr[rid];

		return lo.discount > ro.discount;
	});
	

}

vector <string> orders;

void read_orders()
{
	ifstream ifs("order.txt");
	
	while (ifs)
	{
		string s;
		getline(ifs,s);
		
		if (s.size())
		{
			orders.push_back(s);
		}
	}
}

bool validate_orders()
{
	return find_if_not(orders.begin(), orders.end(), [](const string &s)
														{ return m_fr.find(s) != m_fr.end(); 
														}) == orders.end();
	
}

vector <string> order_items;

void break_set_in_order()
{
	for( auto o : orders )
	{
		copy( m_fr[o].component.begin() , m_fr[o].component.end() , back_inserter(order_items));
	}
}




int main (int argc, char *argv[])
{
	init_menu();
	
	read_orders();
	
	copy(orders.begin(), orders.end(),ostream_iterator<string>(cout,"\n"));
	cout << "====" << endl;
	
	if (!validate_orders())
		cerr << "Error in order file, please check" << endl;
	else
	{
		break_set_in_order();
		copy(order_items.begin(), order_items.end(),ostream_iterator<string>(cout,"\n"));
		cout << "====" << endl;
	}
	
	resource_pool p(order_items);
	
	double sum = 0;
	
	for_each ( sorted_menu.begin() , sorted_menu.end() , [&](auto s)
	{
		const struct food_record &f = m_fr[s];
		vector <string> v = p.find_all_set( f.component );
		if ( v.size() )
		{
			double m = v.size() / f.component.size();
			cerr << m << " * " << f.id << endl;
			sum += (m * f.price);	
		}	
	});
	
	
	cout << "Total: " << sum << endl;
		
	return 0;
}
