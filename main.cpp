#include "pipeline/pipeline.hpp"

#include <iostream>

class AddSys : public ppl::Process<int, float> 
{
public:
	int amt;

	AddSys(int a) : amt(a) {}

	virtual void update(int& data, float& more_data) 
	{
		data += amt;

		abort();
	}

};


int main(int argc, char const *argv[])
{	
	ppl::Pipeline<int, float> test = ppl::Pipeline<int, float>();

	test.pushbk<AddSys>(0, 10);
	test.pushbk<AddSys>(1, 10);
	test.pushbk<AddSys>(2, 10);
	test.insert<AddSys>(0, 3, -10);

	test.for_each([&](ppl::Pipeline<int, float>::id_type id, ppl::Process<int, float>& p)
		{
			std::cout << "Process: " << id << std::endl;
		});

	int data1 = 10;
	float data2 = 0;

	test.update(data1, data2);

	std::cout << "Result: " << data1 << " " << data2 << std::endl;

	return 0;
}