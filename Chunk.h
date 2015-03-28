#ifndef CHUNK_H
#define CHUNK_H

#include <cstdlib>
#include <iostream>
#include <vector>



class Chunk {

    public:      

		Chunk(int pw, int ph, int pl);
		~Chunk();


	private:
		int pw, ph, pl;
};



#endif