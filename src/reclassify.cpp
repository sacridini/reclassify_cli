#include "gdal_priv.h"
#include "cpl_string.h"

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>

using namespace std;

void reclassify(string input_filename,
				string output_filename,
				string image_type,
				vector<float> reclass_in_values,
				vector<float> reclass_out_values);

int main(int argc, char **argv)
{
  bool parallel = false; // TODO
  int idx_diff_in, idx_diff_out;
  string input_filename;
  string output_filename;
  string image_type;
  vector<float> reclass_in_values;
  vector<float> reclass_out_values;

  vector<string> args;
  for (size_t i = 0; i < argc; i++)
	{
	  args.push_back(argv[i]);
	}

  for (size_t i = 0; i < args.size(); i++)
	{
	  if (args.at(i) == "-i" || args.at(i) == "-in") input_filename = args.at(i + 1);
	  if (args.at(i) == "-o" || args.at(i) == "-out") output_filename = args.at(i + 1);
	  if (args.at(i) == "-p") parallel = true;

	  if (args.at(i) == "-t" || args.at(i) == "-type")
		{
		  image_type = args.at(i + 1);
		  if (image_type != "float" &&
			  image_type != "uint8" &&
			  image_type != "uint16" &&
			  image_type != "uint32" &&
			  image_type != "int" &&
			  image_type != "double")
			{
			  cerr << "Wrong Raster Type Format! Valid ones: int, uint8, uint16, uint32, float or double" << endl;
			}
		}

	  if (args.at(i) == "-iv")
		{
		  for (size_t j = 0; j < args.size(); j++)
			{
			  if (args.at(j) == "-iv_end")
				{
				  idx_diff_in = j - i;
				  for (size_t h = 1; h < idx_diff_in; h++)
					{
					  reclass_in_values.push_back(std::stof(args.at(i + h)));
					}
				}
			}
		}

	  if (args.at(i) == "-ov")
		{
		  for (size_t j = 0; j < args.size(); j++)
			{
			  if (args.at(j) == "-ov_end")
				{
				  idx_diff_out = j - i;
				  for (size_t h = 1; h < idx_diff_out; h++)
					{
					  reclass_out_values.push_back(std::stof(args.at(i + h)));
					}
				}
			}
		}

	  if (idx_diff_out != idx_diff_out)
		{
		  cerr << "The number of values to reclassify from input values needs to be the same as the output" << endl;
		}
	}

  cout << "Calling reclassify function on: " << input_filename << endl;

  // Call Reclassify
  reclassify(input_filename,
			 output_filename,
			 image_type,
			 reclass_in_values,
			 reclass_out_values);

  cout << "Finished Reclassify Process." << endl;
}


void reclassify(string input_filename,
				string output_filename,
				string image_type,
				vector<float> reclass_in_values,
				vector<float> reclass_out_values)
{
  GDALDataset* in_ds;
  GDALDataset* out_ds;

  GDALAllRegister();
  GDALDriver *geoTiff;
  GDALDataType gdal_raster_type;

  int nRows, nCols;
  double noData;
  double transform[6];
  const char* proj;
  char **papszOptions = NULL;

  in_ds = (GDALDataset*)GDALOpen(input_filename.c_str(), GA_ReadOnly);

  nRows = in_ds->GetRasterBand(1)->GetYSize();
  nCols = in_ds->GetRasterBand(1)->GetXSize();
  noData = in_ds->GetRasterBand(1)->GetNoDataValue();
  in_ds->GetGeoTransform(transform);
  proj = in_ds->GetProjectionRef();

  geoTiff = GetGDALDriverManager()->GetDriverByName("GTiff");
  papszOptions = CSLSetNameValue(papszOptions, "COMPRESS", "DEFLATE");


  if (image_type == "float")
	{
	  gdal_raster_type = GDT_Float32;
	  float *input_Row = (float*)CPLMalloc(sizeof(float)*nCols);
	  float *output_Row = (float*)CPLMalloc(sizeof(float)*nCols);

	  out_ds = geoTiff->Create(output_filename.c_str(), nCols, nRows, 1, gdal_raster_type, papszOptions);
	  out_ds->SetGeoTransform(transform);
	  out_ds->GetRasterBand(1)->SetNoDataValue(noData);
	  out_ds->SetProjection(proj);

	  for (int i = 0; i < nRows; i++)
		{
		  in_ds->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, input_Row, nCols, 1, gdal_raster_type, 0, 0);
		  for (int j = 0; j < nCols; j++)
			{
			  if (input_Row[j] == (float)noData)
				{
				  output_Row[j] = (float)noData;
				}

			  output_Row[j] = input_Row[j];

			  for (size_t x = 0; x < reclass_in_values.size(); x++)
				{
				  if (input_Row[j] == reclass_in_values.at(x))
					{
					  output_Row[j] = reclass_out_values.at(x);
					}
				}
			}
		  out_ds->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, output_Row, nCols, 1, gdal_raster_type, 0, 0);
		}
	  CPLFree(input_Row);
	  CPLFree(output_Row);
	  GDALClose(in_ds);
	  GDALClose(out_ds);
	  GDALDestroyDriverManager();
	}
  else if (image_type == "double")
	{
	  gdal_raster_type = GDT_Float64;
	  double *input_Row = (double*)CPLMalloc(sizeof(double)*nCols);
	  double *output_Row = (double*)CPLMalloc(sizeof(double)*nCols);

	  out_ds = geoTiff->Create(output_filename.c_str(), nCols, nRows, 1, gdal_raster_type, papszOptions);
	  out_ds->SetGeoTransform(transform);
	  out_ds->GetRasterBand(1)->SetNoDataValue(noData);
	  out_ds->SetProjection(proj);

	  for (int i = 0; i < nRows; i++)
		{
		  in_ds->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, input_Row, nCols, 1, gdal_raster_type, 0, 0);
		  for (int j = 0; j < nCols; j++)
			{
			  if (input_Row[j] == noData)
				{
				  output_Row[j] = noData;
				}

			  output_Row[j] = input_Row[j];

			  for (size_t x = 0; x < reclass_in_values.size(); x++)
				{
				  if (input_Row[j] == reclass_in_values.at(x))
					{
					  output_Row[j] = reclass_out_values.at(x);
					}
				}
			}
		  out_ds->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, output_Row, nCols, 1, gdal_raster_type, 0, 0);
		}
	  CPLFree(input_Row);
	  CPLFree(output_Row);
	  GDALClose(in_ds);
	  GDALClose(out_ds);
	  GDALDestroyDriverManager();
	}
  else if (image_type == "uint8")
	{
	  gdal_raster_type = GDT_Byte;
	  uint8_t *input_Row = (uint8_t*)CPLMalloc(sizeof(uint8_t)*nCols);
	  uint8_t *output_Row = (uint8_t*)CPLMalloc(sizeof(uint8_t)*nCols);

	  out_ds = geoTiff->Create(output_filename.c_str(), nCols, nRows, 1, gdal_raster_type, papszOptions);
	  out_ds->SetGeoTransform(transform);
	  out_ds->GetRasterBand(1)->SetNoDataValue(noData);
	  out_ds->SetProjection(proj);

	  for (int i = 0; i < nRows; i++)
		{
		  in_ds->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, input_Row, nCols, 1, gdal_raster_type, 0, 0);
		  for (int j = 0; j < nCols; j++)
			{
			  if (input_Row[j] == (uint8_t)noData)
				{
				  output_Row[j] = (uint8_t)noData;
				}

			  output_Row[j] = input_Row[j];

			  for (size_t x = 0; x < reclass_in_values.size(); x++)
				{
				  if (input_Row[j] == (uint8_t)reclass_in_values.at(x))
					{
					  output_Row[j] = (uint8_t)reclass_out_values.at(x);
					}
				}
			}
		  out_ds->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, output_Row, nCols, 1, gdal_raster_type, 0, 0);
		}
	  CPLFree(input_Row);
	  CPLFree(output_Row);
	  GDALClose(in_ds);
	  GDALClose(out_ds);
	  GDALDestroyDriverManager();
	}
  else if (image_type == "uint16")
	{
	  gdal_raster_type = GDT_UInt16;
	  uint16_t *input_Row = (uint16_t*)CPLMalloc(sizeof(uint16_t)*nCols);
	  uint16_t *output_Row = (uint16_t*)CPLMalloc(sizeof(uint16_t)*nCols);

	  out_ds = geoTiff->Create(output_filename.c_str(), nCols, nRows, 1, gdal_raster_type, papszOptions);
	  out_ds->SetGeoTransform(transform);
	  out_ds->GetRasterBand(1)->SetNoDataValue(noData);
	  out_ds->SetProjection(proj);

	  for (int i = 0; i < nRows; i++)
		{
		  in_ds->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, input_Row, nCols, 1, gdal_raster_type, 0, 0);
		  for (int j = 0; j < nCols; j++)
			{
			  if (input_Row[j] == (uint16_t)noData)
				{
				  output_Row[j] = (uint16_t)noData;
				}

			  output_Row[j] = input_Row[j];

			  for (size_t x = 0; x < reclass_in_values.size(); x++)
				{
				  if (input_Row[j] == (uint16_t)reclass_in_values.at(x))
					{
					  output_Row[j] = (uint16_t)reclass_out_values.at(x);
					}
				}
			}
		  out_ds->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, output_Row, nCols, 1, gdal_raster_type, 0, 0);
		}
	  CPLFree(input_Row);
	  CPLFree(output_Row);
	  GDALClose(in_ds);
	  GDALClose(out_ds);
	  GDALDestroyDriverManager();
	}
  else if (image_type == "uint32")
	{
	  gdal_raster_type = GDT_UInt32;
	  uint32_t *input_Row = (uint32_t*)CPLMalloc(sizeof(uint32_t)*nCols);
	  uint32_t *output_Row = (uint32_t*)CPLMalloc(sizeof(uint32_t)*nCols);

	  out_ds = geoTiff->Create(output_filename.c_str(), nCols, nRows, 1, gdal_raster_type, papszOptions);
	  out_ds->SetGeoTransform(transform);
	  out_ds->GetRasterBand(1)->SetNoDataValue(noData);
	  out_ds->SetProjection(proj);

	  for (int i = 0; i < nRows; i++)
		{
		  in_ds->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, input_Row, nCols, 1, gdal_raster_type, 0, 0);
		  for (int j = 0; j < nCols; j++)
			{
			  if (input_Row[j] == (uint32_t)noData)
				{
				  output_Row[j] = (uint32_t)noData;
				}

			  output_Row[j] = input_Row[j];

			  for (size_t x = 0; x < reclass_in_values.size(); x++)
				{
				  if (input_Row[j] == (uint32_t)reclass_in_values.at(x))
					{
					  output_Row[j] = (uint32_t)reclass_out_values.at(x);
					}
				}
			}
		  out_ds->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, output_Row, nCols, 1, gdal_raster_type, 0, 0);
		}
	  CPLFree(input_Row);
	  CPLFree(output_Row);
	  GDALClose(in_ds);
	  GDALClose(out_ds);
	  GDALDestroyDriverManager();
	}
  else if (image_type == "int")
	{
	  gdal_raster_type = GDT_Int32;
	  int *input_Row = (int*)CPLMalloc(sizeof(int)*nCols);
	  int *output_Row = (int*)CPLMalloc(sizeof(int)*nCols);

	  out_ds = geoTiff->Create(output_filename.c_str(), nCols, nRows, 1, gdal_raster_type, papszOptions);
	  out_ds->SetGeoTransform(transform);
	  out_ds->GetRasterBand(1)->SetNoDataValue(noData);
	  out_ds->SetProjection(proj);

	  for (int i = 0; i < nRows; i++)
		{
		  in_ds->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, input_Row, nCols, 1, gdal_raster_type, 0, 0);
		  for (int j = 0; j < nCols; j++)
			{
			  if (input_Row[j] == (int)noData)
				{
				  output_Row[j] = (int)noData;
				}

			  output_Row[j] = input_Row[j];

			  for (size_t x = 0; x < reclass_in_values.size(); x++)
				{
				  if (input_Row[j] == (int)reclass_in_values.at(x))
					{
					  output_Row[j] = (int)reclass_out_values.at(x);
					}
				}
			}
		  out_ds->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, output_Row, nCols, 1, gdal_raster_type, 0, 0);
		}
	  CPLFree(input_Row);
	  CPLFree(output_Row);
	  GDALClose(in_ds);
	  GDALClose(out_ds);
	  GDALDestroyDriverManager();
	}
}
