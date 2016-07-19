#include <iostream.h>
#include <fstream.h>

int main()
{
        int i = 0;
	while(1)
	{
		fstream file;
		file.open("alpha.txt", ios::out);
		file << "a";
		file.close();
	}

	return 0;
}
