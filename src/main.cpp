#include "pch.hpp"
#include "demo_octree.hpp"

int main(int argc, const char* argv[])
{
	cs350::demo_octree demo;
	demo.create();
	while (demo.update()){}
	demo.destroy();
	return 0;
}