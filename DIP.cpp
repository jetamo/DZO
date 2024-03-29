// DIP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cmath>
#include <algorithm>
#include <vector>


void tutorial() {
	cv::Mat src_8uc3_img = cv::imread("images/lena.png", CV_LOAD_IMAGE_COLOR); // load color image from file system to Mat variable, this will be loaded using 8 bits (uchar)

	if (src_8uc3_img.empty()) {
		printf("Unable to read input file (%s, %d).", __FILE__, __LINE__);
	}

	//cv::imshow( "LENA", src_8uc3_img );

	cv::Mat gray_8uc1_img; // declare variable to hold grayscale version of img variable, gray levels wil be represented using 8 bits (uchar)
	cv::Mat gray_32fc1_img; // declare variable to hold grayscale version of img variable, gray levels wil be represented using 32 bits (float)

	cv::cvtColor(src_8uc3_img, gray_8uc1_img, CV_BGR2GRAY); // convert input color image to grayscale one, CV_BGR2GRAY specifies direction of conversion
	gray_8uc1_img.convertTo(gray_32fc1_img, CV_32FC1, 1.0 / 255.0); // convert grayscale image from 8 bits to 32 bits, resulting values will be in the interval 0.0 - 1.0

	int x = 10, y = 15; // pixel coordinates

	uchar p1 = gray_8uc1_img.at<uchar>(y, x); // read grayscale value of a pixel, image represented using 8 bits
	float p2 = gray_32fc1_img.at<float>(y, x); // read grayscale value of a pixel, image represented using 32 bits
	cv::Vec3b p3 = src_8uc3_img.at<cv::Vec3b>(y, x); // read color value of a pixel, image represented using 8 bits per color channel

	// print values of pixels
	printf("p1 = %d\n", p1);
	printf("p2 = %f\n", p2);
	printf("p3[ 0 ] = %d, p3[ 1 ] = %d, p3[ 2 ] = %d\n", p3[0], p3[1], p3[2]);

	gray_8uc1_img.at<uchar>(y, x) = 0; // set pixel value to 0 (black)

	// draw a rectangle
	cv::rectangle(gray_8uc1_img, cv::Point(65, 84), cv::Point(75, 94),
		cv::Scalar(50), CV_FILLED);

	// declare variable to hold gradient image with dimensions: width= 256 pixels, height= 50 pixels.
	// Gray levels wil be represented using 8 bits (uchar)
	cv::Mat gradient_8uc1_img(50, 256, CV_8UC1);

	// For every pixel in image, assign a brightness value according to the x coordinate.
	// This wil create a horizontal gradient.
	for (int y = 0; y < gradient_8uc1_img.rows; y++) {
		for (int x = 0; x < gradient_8uc1_img.cols; x++) {
			gradient_8uc1_img.at<uchar>(y, x) = x;
		}
	}

	// diplay images
	cv::imshow("Gradient", gradient_8uc1_img);
	cv::imshow("Lena gray", gray_8uc1_img);
	cv::imshow("Lena gray 32f", gray_32fc1_img);

	cv::waitKey(0); // wait until keypressed
	return;
}

void gamma_correction() {
	cv::Mat moon_img = cv::imread("images/moon.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	moon_img.convertTo(moon_img, CV_32FC1, 1.0 / 255.0);
	cv::Mat corrected_moon_img(moon_img.rows, moon_img.cols, CV_32FC1);

	float factor = 0.55f;

	for (int y = 0; y < moon_img.rows; y++) {
		for (int x = 0; x < moon_img.cols; x++) {
			corrected_moon_img.at<float>(y, x) = cv::pow(moon_img.at<float>(y, x), 1/factor);
		}
	}
	cv::imshow("Before correction", moon_img);
	cv::imshow("After correction", corrected_moon_img);
	
	cv::waitKey(0); 
	return;
}


void convolution(bool lena) {
	
	cv::Mat lena_img;
	if (lena)
		lena_img = cv::imread("images/lena.png", CV_LOAD_IMAGE_GRAYSCALE);
	else
		lena_img = cv::imread("images/valve.png", CV_LOAD_IMAGE_GRAYSCALE);

	lena_img.convertTo(lena_img, CV_32FC1, 1.0 / 255.0);

	cv::Mat blured_lena_img(lena_img.rows, lena_img.cols, CV_32FC1);
	cv::Mat G_y(lena_img.rows, lena_img.cols, CV_32FC1);
	cv::Mat G(lena_img.rows, lena_img.cols, CV_32FC1);

	float boxMat3x3[9] =
	{
		1.f/9.f, 1.f / 9.f, 1.f / 9.f,
		1.f / 9.f, 1.f / 9.f, 1.f / 9.f,
		1.f / 9.f, 1.f / 9.f, 1.f / 9.f
	};

	float gaussianMat3x3[9] =
	{
		1.f / 16.f, 2.f / 16.f, 1.f / 16.f,
		2.f / 16.f, 4.f / 16.f, 2.f / 16.f,
		1.f / 16.f, 2.f / 16.f, 1.f / 16.f
	};

	float Gx[9] =
	{
		1.f, 0.f, -1.f,
		2.f, 0.f, -2.f,
		1.f, 0.f, -1.f
	};

	float Gy[9] =
	{
		1.f, 2.f, 1.f,
		0.f, 0.f, 0.f,
		-1.f, -2.f, -1.f
	};

	float gaussianMat5x5[25] =
	{
		1.f/256.f, 4.f/256.f,  6.f/256.f,  4.f/256.f,  1.f/256.f,
		4.f/256.f, 16.f/256.f, 24.f/256.f, 16.f/256.f, 4.f/256.f,
		6.f/256.f, 24.f/256.f, 36.f/256.f, 24.f/256.f, 6.f/256.f,
		4.f/256.f, 16.f/256.f, 24.f/256.f, 16.f/256.f, 4.f/256.f,
		1.f/256.f, 4.f/256.f,  6.f/256.f,  4.f/256.f,  1.f/256.f
	};

	cv::Mat mask = cv::Mat(5, 5, CV_32FC1, gaussianMat5x5);
	if(!lena)
		mask = cv::Mat(3, 3, CV_32FC1, Gx);


	for (int y = mask.rows / 2; y < lena_img.rows - mask.rows / 2; y++) {
		for (int x = mask.cols / 2; x < lena_img.cols - mask.cols / 2; x++) {
			float value = 0;
			for (int my = 0; my < mask.rows; my++) {
				for (int mx = 0; mx < mask.cols; mx++) {
					value += mask.at<float>(my, mx) * lena_img.at<float>(y + my - mask.rows / 2, x + mx - mask.cols / 2);
				}
			}

			blured_lena_img.at<float>(y, x) = value;
		}
	}
	


	if (lena) {
		cv::imshow("Before convolution", lena_img);
		cv::imshow("After convolution", blured_lena_img);
	}
	else {

		mask = cv::Mat(3, 3, CV_32FC1, Gy);
		for (int y = mask.rows / 2; y < lena_img.rows - mask.rows / 2; y++) {
			for (int x = mask.cols / 2; x < lena_img.cols - mask.cols / 2; x++) {
				float value = 0;
				for (int my = 0; my < mask.rows; my++) {
					for (int mx = 0; mx < mask.cols; mx++) {
						value += mask.at<float>(my, mx) * lena_img.at<float>(y + my - mask.rows / 2, x + mx - mask.cols / 2);
					}
				}

				G_y.at<float>(y, x) = value;
			}
		}
		pow(blured_lena_img, 2, blured_lena_img);
		pow(G_y, 2, G_y);
		sqrt(blured_lena_img + G_y, G);
		cv::imshow("Original image", lena_img);
		cv::imshow("Edge detection", G);
		//cv::imshow("Gx", blured_lena_img);
		//cv::imshow("Gy", G_y);
	}



	cv::waitKey(0);
	return;
}

void anisotrpic_filtration() {
	cv::Mat valve_img = cv::imread("images/valve.png", CV_LOAD_IMAGE_GRAYSCALE);

	valve_img.convertTo(valve_img, CV_64F, 1.0 / 255.0);

	cv::Mat original_valve_img = cv::imread("images/valve.png", CV_LOAD_IMAGE_GRAYSCALE);

	float lambda = 0.1f;
	float sigma = 0.015f;
	int iterations = 500;

	cv::Mat temp_img(valve_img.rows, valve_img.cols, CV_64F);

	for (int i = 0; i < iterations; i++) {
		for (int y = 1; y < valve_img.rows - 1; y++) {
			for (int x = 1; x < valve_img.cols - 1; x++) {

				double I_N = valve_img.at<double>(y, x-1);
				double I_S = valve_img.at<double>(y, x+1);
				double I_E = valve_img.at<double>(y+1, x);
				double I_W = valve_img.at<double>(y-1, x);

				double delta_I_N = valve_img.at<double>(y, x - 1) - valve_img.at<double>(y, x);
				double delta_I_S = valve_img.at<double>(y, x + 1) - valve_img.at<double>(y, x);
				double delta_I_E = valve_img.at<double>(y + 1, x) - valve_img.at<double>(y, x);
				double delta_I_W = valve_img.at<double>(y - 1, x) - valve_img.at<double>(y, x);


				double C_N = exp(-1 * ((delta_I_N * delta_I_N) / (sigma * sigma)));
				double C_S = exp(-1 * ((delta_I_S * delta_I_S) / (sigma * sigma)));
				double C_E = exp(-1 * ((delta_I_E * delta_I_E) / (sigma * sigma)));
				double C_W = exp(-1 * ((delta_I_W * delta_I_W) / (sigma * sigma)));


				temp_img.at<double>(y, x) = valve_img.at<double>(y, x) * (1 - lambda * (C_N + C_S + C_E + C_W)) +
					lambda * (C_N * I_N + C_S * I_S + C_E * I_E + C_W * I_W);
			}
		}
		temp_img.copyTo(valve_img);
	}


	cv::imshow("Before filtration", original_valve_img);
	cv::imshow("After filtration", valve_img);

	cv::waitKey(0);
	return;
}

double inv_lerp(double v, double a, double b) {
	return (v-a)/(b-a);
}

cv::Mat discrete_fourier_transform(bool show) {

	cv::Mat lena_img = cv::imread("images/lena64.png", CV_LOAD_IMAGE_GRAYSCALE);

	lena_img.convertTo(lena_img, CV_64F, 1.0 / 255.0);

	cv::Mat DFT_BGR2GRAY;
	cv::Mat DFT_BGR2GRAY_result;
	cv::Mat DFT_BGR2GRAY_phase;


	lena_img.convertTo(DFT_BGR2GRAY, CV_64FC1, 1.0 / 255.0);
	lena_img.convertTo(DFT_BGR2GRAY_result, CV_64FC1, 1.0 / 255.0);
	lena_img.convertTo(DFT_BGR2GRAY_phase, CV_64FC1, 1.0 / 255.0);


	int height = DFT_BGR2GRAY.rows;
	int width = DFT_BGR2GRAY.cols;

	for (int l = 0; l < height; l++)
		for (int k = 0; k < width; k++)
		{
			double real = 0.0;
			double comp = 0.0;


			for (int n = 0; n < height; n++)
				for (int m = 0; m < width; m++)
				{
					double fmn = DFT_BGR2GRAY.at<double>(n, m);
					double argument = 2 * CV_PI * (((m * k) / (double)height) + ((n * l) / (double)width));
					argument *= -1;
					real += cos(argument) * fmn;
					comp += sin(argument) * fmn;
				}

			double phase = atan(comp / real);
			double spec = sqrt(real * real + comp * comp);
			double power = log(spec);
			DFT_BGR2GRAY_result.at<double>(l, k) = power;
			DFT_BGR2GRAY_phase.at<double>(l, k) = phase;
		}


	double min__ = 0.0;
	double max__ = 0.0;
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
		{
			double value = DFT_BGR2GRAY_result.at<double>(y, x);
			if (value < min__)
				min__ = value;

			if (value > max__)
				max__ = value;
		}
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
		{
			double value = DFT_BGR2GRAY_result.at<double>(y, x);
			DFT_BGR2GRAY_result.at<double>(y, x) = inv_lerp(value, min__, max__);
		}


	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
		{
			if (x >= width / 2 && y >= height / 2)
			{
				double tmp = DFT_BGR2GRAY_result.at<double>(y, x);
				int x_ = x - width / 2;
				int y_ = y - height / 2;

				std::swap(DFT_BGR2GRAY_result.at<double>(y, x), DFT_BGR2GRAY_result.at<double>(y_, x_));
			}

			if (x >= width / 2 && y < height / 2)
			{
				double tmp = DFT_BGR2GRAY_result.at<double>(y, x);
				int x_ = x - width / 2;
				int y_ = y + height / 2;

				std::swap(DFT_BGR2GRAY_result.at<double>(y, x), DFT_BGR2GRAY_result.at<double>(y_, x_));
			}
		}

	int _scale = 4;

	if (show) {
		resize(lena_img, lena_img, cv::Size(64 * _scale, 64 * _scale), cv::INTER_LINEAR);
		resize(DFT_BGR2GRAY_result, DFT_BGR2GRAY_result, cv::Size(64 * _scale, 64 * _scale), cv::INTER_LINEAR);
		resize(DFT_BGR2GRAY_phase, DFT_BGR2GRAY_phase, cv::Size(64 * _scale, 64 * _scale), cv::INTER_LINEAR);
		imshow("original image", lena_img);
		imshow("power", DFT_BGR2GRAY_result);
		imshow("phase", DFT_BGR2GRAY_phase);

		cv::waitKey(0);
	}


	return DFT_BGR2GRAY_result;
}


struct RLDUserData {
	cv::Mat& src_8uc3_img;
	cv::Mat& undistorted_8uc3_img;
	int K1;
	int K2;

	RLDUserData(const int K1, const int K2, cv::Mat& src_8uc3_img, cv::Mat& undistorted_8uc3_img) : K1(K1), K2(K2), src_8uc3_img(src_8uc3_img), undistorted_8uc3_img(undistorted_8uc3_img) {

	}
};

int round_to_closest(float x, int max) {
	int x_r = static_cast<int>(std::round(x));

	if (x_r < 0)
		x_r = 0;
	if (x_r > max - 1)
		x_r = max - 1;
	return x_r;
}

void geom_dist(cv::Mat& src_8uc3_img, cv::Mat& dst_8uc3_img, bool bili, double K1 = -1.0, double K2 = -1.0)
{
	cv::transpose(src_8uc3_img, src_8uc3_img);
	cv::transpose(dst_8uc3_img, dst_8uc3_img);
	std::vector<std::tuple<float, float>> coords;

	for (int y = 0; y < src_8uc3_img.rows; y++) {
		for (int x = 0; x < src_8uc3_img.cols; x++) {
			int c_u = dst_8uc3_img.cols / 2;
			int c_v = dst_8uc3_img.rows / 2;
			float R = sqrt(pow(c_u, 2) + pow(c_v, 2));
			float x_ = (x - c_u) / R;
			float y_ = (y - c_v) / R;
			float r_2 = pow(x_, 2) + pow(y_, 2);
			float sr2 = 1 + (K1 * r_2) + (K2 * pow(r_2, 2));
			//coords.push_back(std::tuple<float, float>(x_, y_));
			float x_d = (x - c_u) * pow(sr2, -1) + c_u;
			float y_d = (y - c_v) * pow(sr2, -1) + c_v;
			dst_8uc3_img.at<cv::Vec3b>(y, x) = src_8uc3_img.at<cv::Vec3b>(round_to_closest(y_d, dst_8uc3_img.rows), round_to_closest(x_d, dst_8uc3_img.cols));
		}
	}


	cv::transpose(src_8uc3_img, src_8uc3_img);
	cv::transpose(dst_8uc3_img, dst_8uc3_img);
}



void apply_rld(int id, void* user_data)
{
	RLDUserData* rld_user_data = (RLDUserData*)user_data;

	geom_dist(rld_user_data->src_8uc3_img, rld_user_data->undistorted_8uc3_img, !false, rld_user_data->K1 / 100.0, rld_user_data->K2 / 100.0);
	cv::imshow("Geom Dist", rld_user_data->undistorted_8uc3_img);
}

int ex_rld()
{
	cv::Mat src_8uc3_img, geom_8uc3_img;
	RLDUserData rld_user_data(3.0, 1.0, src_8uc3_img, geom_8uc3_img);

	src_8uc3_img = cv::imread("images/distorted_window.jpg", cv::IMREAD_COLOR);
	//src_8uc3_img = cv::imread("images/distorted_panorama.jpg", cv::IMREAD_COLOR);
	if (src_8uc3_img.empty())
	{
		printf("Unable to load image!\n");
		exit(-1);
	}

	cv::namedWindow("Original Image");
	cv::imshow("Original Image", src_8uc3_img);

	geom_8uc3_img = src_8uc3_img.clone();
	apply_rld(0, (void*)&rld_user_data);

	cv::namedWindow("Geom Dist");
	cv::imshow("Geom Dist", geom_8uc3_img);

	cv::createTrackbar("K1", "Geom Dist", &rld_user_data.K1, 100, apply_rld, &rld_user_data);
	cv::createTrackbar("K2", "Geom Dist", &rld_user_data.K2, 100, apply_rld, &rld_user_data);

	cv::waitKey(0);

	return 0;
}

void perspective_transform()
{

	cv::Mat school_img = cv::imread("images/vsb.jpg", CV_LOAD_IMAGE_COLOR);
	cv::Mat flag_img = cv::imread("images/flag.png", CV_LOAD_IMAGE_COLOR);
	
	int A1_[2] = {0, 0};
	int A2_[2] = {323, 0};
	int A3_[2] = {323, 215};
	int A4_[2] = {0, 215};

	int A1[2] = {69, 107};
	int A2[2] = {227, 76};
	int A3[2] = {228, 122};
	int A4[2] = {66, 134};

	cv::Mat left = (cv::Mat_<float>(8, 8) << A1_[0], A1_[1], 1, 0, 0, 0, -A1_[0] * A1[0], -A1_[1] * A1[0],
											 0, 0, 0, A1_[0], A1_[1], 1, -A1_[0] * A1[1], -A1_[1] * A1[1],

											 A2_[0], A2_[1], 1, 0, 0, 0, -A2_[0] * A2[0], -A2_[1] * A2[0],
											 0, 0, 0, A2_[0], A2_[1], 1, -A2_[0] * A2[1], -A2_[1] * A2[1],

											 A3_[0], A3_[1], 1, 0, 0, 0, -A3_[0] * A3[0], -A3_[1] * A3[0],
											 0, 0, 0, A3_[0], A3_[1], 1, -A3_[0] * A3[1], -A3_[1] * A3[1],

											 A4_[0], A4_[1], 1, 0, 0, 0, -A4_[0] * A4[0], -A4_[1] * A4[0],
											 0, 0, 0, A4_[0], A4_[1], 1, -A4_[0] * A4[1], -A4_[1] * A4[1]);

	cv::Mat right = (cv::Mat_<float>(8, 1) << A1[0], A1[1], A2[0], A2[1], A3[0], A3[1], A4[0], A4[1]);


	cv::Mat solved = cv::Mat(8, 1, CV_32F);


	cv::solve(left, right, solved);

	std::cout << solved;

	cv::imshow("original", school_img);
	cv::imshow("flag", flag_img);

	for (int y = 0; y < flag_img.rows; y++) {
		for (int x = 0; x < flag_img.cols; x++) {

			int x_ = (solved.at<float>(0, 0) * x + solved.at<float>(1, 0) * y + solved.at<float>(2, 0))
				/ (solved.at<float>(6, 0) * x + solved.at<float>(7, 0) * y + 1);

			int y_ = (solved.at<float>(3, 0) * x + solved.at<float>(4, 0) * y + solved.at<float>(5, 0))
				/ (solved.at<float>(6, 0) * x + solved.at<float>(7, 0) * y + 1);

			school_img.at<cv::Vec3b>(y_, x_) = flag_img.at<cv::Vec3b>(y, x);
		}
	}

	cv::imshow("with flag", school_img);
	cv::waitKey();

	cv::waitKey(0);

	return;
}

void histogram_equalization() {
	cv::Mat scenery_img = cv::imread("images/uneq.jpg", CV_LOAD_IMAGE_GRAYSCALE);

	cv::Mat scenery_img_new = cv::imread("images/uneq.jpg", CV_LOAD_IMAGE_GRAYSCALE);

	std::vector<int> histogram_vals(256, 0);

	for (int y = 0; y < scenery_img.rows; y++) {
		for (int x = 0; x < scenery_img.cols; x++) {
			int brightness = scenery_img.at<uchar>(y, x);
			histogram_vals[brightness] += 1;
		}
	}

	std::vector<int> distributed_histogram_vals(256, 0);

	for (int i = 0; i < 256; i++) {
		int sum = 0;
		for (int j = 0; j < i; j++) {
			sum += histogram_vals[j] ;
		}
		distributed_histogram_vals[i] = sum;
	}

	int min_value = 0;

	for (int i = 0; i < 256; i++) {
		if (distributed_histogram_vals[i] > 0) {
			min_value = i;
			break;
		}
	}

	std::vector<int> LUT(256, 0);

	for (int i = 0; i < 256; i++) {
		int new_val = round(((distributed_histogram_vals[i] - min_value) * (256 - 1) / (scenery_img.cols * scenery_img.rows - min_value)));
		LUT[i] = new_val;
	}

	cv::Mat histogram_image(400, 512, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Mat distributed_histogram_image(400, 512, CV_8UC3, cv::Scalar(0, 0, 0));

	cv::Mat histogram_image_new(400, 512, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Mat distributed_histogram_image_new(400, 512, CV_8UC3, cv::Scalar(0, 0, 0));

	for (int y = 0; y < scenery_img.rows; y++) {
		for (int x = 0; x < scenery_img.cols; x++) {
			int brightness = scenery_img.at<uchar>(y, x);
			scenery_img_new.at<uchar>(y, x) = LUT[brightness];
		}
	}

	std::vector<int> new_histogram_vals(256, 0);

	for (int y = 0; y < scenery_img_new.rows; y++) {
		for (int x = 0; x < scenery_img_new.cols; x++) {
			int brightness = scenery_img_new.at<uchar>(y, x);
			new_histogram_vals[brightness] += 1;
		}
	}

	std::vector<int> new_distributed_histogram_vals(256, 0);

	for (int i = 0; i < 256; i++) {
		int sum = 0;
		for (int j = 0; j < i; j++) {
			sum += new_histogram_vals[j];
		}
		new_distributed_histogram_vals[i] = sum;
	}

	cv::Mat h1(histogram_vals);
	cv::Mat h2(distributed_histogram_vals);
	cv::Mat h3(new_histogram_vals);
	cv::Mat h4(new_distributed_histogram_vals);


	cv::normalize(h1, h1, 0, 400, cv::NORM_MINMAX, -1, cv::Mat());
	cv::normalize(h2, h2, 0, 400, cv::NORM_MINMAX, -1, cv::Mat());
	cv::normalize(h3, h3, 0, 400, cv::NORM_MINMAX, -1, cv::Mat());
	cv::normalize(h4, h4, 0, 400, cv::NORM_MINMAX, -1, cv::Mat());

	for (int i = 0; i < 256; i++)
	{
		rectangle(histogram_image, cv::Point(2 * i, histogram_image.rows - histogram_vals[i]),
			cv::Point(2 * (i + 1), histogram_image.rows), cv::Scalar(255, 255, 255));
		rectangle(distributed_histogram_image, cv::Point(2 * i, histogram_image.rows - distributed_histogram_vals[i]),
			cv::Point(2 * (i + 1), histogram_image.rows), cv::Scalar(255, 255, 255));

		rectangle(histogram_image_new, cv::Point(2 * i, histogram_image.rows - new_histogram_vals[i]),
			cv::Point(2 * (i + 1), histogram_image.rows), cv::Scalar(255, 255, 255));
		rectangle(distributed_histogram_image_new, cv::Point(2 * i, histogram_image.rows - new_distributed_histogram_vals[i]),
			cv::Point(2 * (i + 1), histogram_image.rows), cv::Scalar(255, 255, 255));
	}

	imshow("Histogram origninal", histogram_image);
	imshow("Histogram cumultative", distributed_histogram_image);
	imshow("Histogram equalized", histogram_image_new);
	imshow("Histogram comultative equalized", distributed_histogram_image_new);
	cv::imshow("Original image", scenery_img);
	cv::imshow("Equalized image", scenery_img_new);
	cv::waitKey();
	return;
}



int main()
{
	//tutorial(); 
	//gamma_correction(); 
	convolution(true); 
	//anisotrpic_filtration(); 
	//discrete_fourier_transform(true); 
	//ex_rld(); //geom distortion 
	//perspective_transform();
	//histogram_equalization();
	//convolution(false); //edge detection 
	return 0;
}
