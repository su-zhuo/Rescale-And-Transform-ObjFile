#include "objmtl.h"
#include <Eigen/Eigen>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

int get_filenames(const std::string& dir, std::vector<std::string>& filenames)
{
	fs::path path(dir);
	if (!fs::exists(path))
	{
		return -1;
	}

	fs::directory_iterator end_iter;
	for (fs::directory_iterator iter(path); iter != end_iter; ++iter)
	{
		/*	if (fs::is_regular_file(iter->status()))
		{
		filenames.push_back(iter->path().string());
		}*/

		if (fs::is_directory(iter->status()))
		{
			get_filenames(iter->path().string(), filenames);
			filenames.push_back(iter->path().string());
		}
	}

	return filenames.size();
}



int main()
{

	int file_number;
	std::vector<std::string> filenames;
	file_number = get_filenames("../results", filenames);
	printf("number is : %d\n", file_number);
	for (int i = 0; i < file_number; i++)
	{
		filenames[i] = filenames[i].substr(0, 10) + '/' + filenames[i].substr(11, 17) + '/' + filenames[i].substr(11, 15) + ".obj";
		std::cout << filenames[i] << std::endl;
	}
	for (int file_idx = 0; file_idx < filenames.size(); file_idx++)
	{
		Objmtl mesh;
		std::string filename = filenames[file_idx];//"../results/126111539895949-h/126111539895949.obj";

		read_objmtl(filename, mesh);

		printf("vertx.num is : %d \n", mesh.verts.size());
		printf("face.num is : %d \n", mesh.faces.size());
		printf("feature.num is : %d \n", mesh.texture.size());
		std::cout << mesh.mtllibName << "\n" << std::endl;


		Eigen::Matrix4f RT = Eigen::Matrix4f::Identity();
		RT << 0.96, -0.01, 0.27, 0.73,
			-0.07, -0.97, 0.21, 1.58,
			0.26, -0.22, -0.94, 1.18,
			0.00, 0.00, 0.00, 1.00;
		Eigen::Matrix<float, 4, 1> v_coor = Eigen::Matrix<float, 1, 4>::Ones();

		float scale = mesh.bbx[1].y - mesh.bbx[0].y;
		for (int i = 0; i < mesh.verts.size(); i++)
		{

			mesh.verts[i].x = (mesh.verts[i].x - mesh.massCenter.x) / scale * 1.73;
			mesh.verts[i].y = (mesh.verts[i].y - mesh.massCenter.y) / scale * 1.73;
			mesh.verts[i].z = (mesh.verts[i].z - mesh.massCenter.z) / scale * 1.73;

			v_coor(0, 0) = mesh.verts[i].x;
			v_coor(1, 0) = mesh.verts[i].y;
			v_coor(2, 0) = mesh.verts[i].z;
			v_coor(3, 0) = 1.0;
			v_coor = RT * v_coor;

			mesh.verts[i].x = v_coor(0, 0) / v_coor(3, 0);
			mesh.verts[i].y = v_coor(1, 0) / v_coor(3, 0);
			mesh.verts[i].z = v_coor(2, 0) / v_coor(3, 0);


		}
		std::string output = filenames[file_idx].substr(0, 29) + filenames[file_idx].substr(11, 15) + ".obj";
		std::cout << output << std::endl;
		Store_objmtl(output, mesh);
	}
	return 0;
}

