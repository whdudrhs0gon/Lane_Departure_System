#include "cv.hpp"
#include <iostream>


using namespace cv;
using namespace std;


int main() {
	vector<String> vec;
	String a = "1.mp4";
	String b = "2.mp4";
	String c = "3.mp4";
	String d = "4.mp4";
	String e = "5.mp4";
	String f = "6.mp4";
	vec.push_back(a);
	vec.push_back(b);
	vec.push_back(c);
	vec.push_back(d);
	vec.push_back(e);
	vec.push_back(f);

	for (int video_number = 0; video_number < vec.size(); video_number++)
	{
		VideoCapture cap(vec[video_number]);
		int fps = cap.get(CAP_PROP_FPS);

		// Parameters and Set ROI
		Mat frame, result, gray, blur, edge, Left_roi, Right_roi, Center_roi;

		Rect Left(Point(250, 300), Point(365, 390));
		Mat bgLeft(Size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT)), CV_8UC1, Scalar(0));
		rectangle(bgLeft, Left, Scalar(255), -1);

		Rect Right(Point(485, 300), Point(600, 390));
		Mat bgRight(Size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT)), CV_8UC1, Scalar(0));
		rectangle(bgRight, Right, Scalar(255), -1);

		Rect Center(Point(375, 300), Point(475, 390));
		Mat bgCenter(Size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT)), CV_8UC1, Scalar(0));
		rectangle(bgCenter, Center, Scalar(255), -1);

		// HoughlinesP's parameters
		Point p1, p2;
		vector <Vec4i> lines;
		float slope, x, y;
		float sum_x1, sum_x2, sum_y1, sum_y2;
		int count_x1, count_x2, count_y1, count_y2;
		int x1, y1, x2, y2;

		// Draw Imaginary_line 
		int count_left_line = 0;
		int left_temp_x1, left_temp_y1, left_temp_x2, left_temp_y2;
		bool Imaginary_right_line_flag = false;
		bool draw_right = false;

		int count_right_line = 0;
		int right_temp_x1, right_temp_y1, right_temp_x2, right_temp_y2;
		bool Imaginary_left_line_flag = false;
		bool draw_left = false;

		bool draw_center = false;

		// LineDeparture's parameter
		int leftCur_x = 0, leftPrev_x = 1;
		int rightCur_x = 0, rightPrev_x = 1;
		int centerCur_x = 0, centerPrev_x = 1;
		int center_point = 0;
		int safe_count1 = 0, warning_count1 = 0, safe_count2 = 0, warning_count2 = 0;
		bool alarm_flag = true;
		int left_detection = 0, right_detection = 0, center_detection = 0;


		while (1)
		{
			if (cap.grab() == 0) break;
			cap.retrieve(frame);
			result = frame.clone();
			cvtColor(result, gray, CV_BGR2GRAY);
			GaussianBlur(gray, blur, Size(5, 5), 1.5);
			Canny(blur, edge, 40, 80, 3);

			rectangle(result, Left, Scalar(255, 0, 0), 3);
			rectangle(result, Right, Scalar(0, 0, 255), 3);
			rectangle(result, Center, Scalar(0, 255, 0), 3);

			// Detect left_line //
			edge.copyTo(Left_roi, bgLeft);
			HoughLinesP(Left_roi, lines, 1, CV_PI / 180, 40, 60, 150);

			// Calculate slope of the line
			sum_x1 = sum_x2 = sum_y1 = sum_y2 = 0;
			count_x1 = count_x2 = count_y1 = count_y2 = 0;
			for (int i = 0; i < lines.size(); i++) {
				Vec4i l = lines[i];

				x = l[2] - l[0];
				y = l[3] - l[1];

				if (x == 0) {
					slope = 100.0f;
				}
				else {
					slope = (-1)*(y / float(x));
				}

				if (0.3f < slope and slope < 2.0f) {
					sum_x1 += l[0];
					sum_y1 += l[1];
					sum_x2 += l[2];
					sum_y2 += l[3];
					count_x1 += 1;
					count_x2 += 1;
					count_y1 += 1;
					count_y2 += 1;
				}
			}

			// Line merging
			x1 = sum_x1 / count_x1;
			y1 = sum_y1 / count_y1;
			x2 = sum_x2 / count_x2;
			y2 = sum_y2 / count_y2;



			// Store to draw Imaginary_right_line
			left_temp_x1 = x1;
			left_temp_y1 = y1;
			left_temp_x2 = x2;
			left_temp_y2 = y2;
			//
			


			// if left_line is detected
			if (x1 >= 250 and x1 <= 365)
			{
				draw_left = true;

				// Draw left_line
				line(result, Point(x1, y1), Point(x2, y2), Scalar(0, 255, 0), 3, 8);

				// Store lower-left point in leftCur_x to find center point
				leftCur_x = x1;
			}
			else
			{
				draw_left = false;
				leftCur_x = leftPrev_x;  // if left_line is not detected, store previous x1 in leftCur_x
			}

			left_detection = lines.size();




			//  Draw right_line //
			edge.copyTo(Right_roi, bgRight);
			HoughLinesP(Right_roi, lines, 1, CV_PI / 180, 50, 60, 150);

			// Calculate slope of the line
			sum_x1 = sum_x2 = sum_y1 = sum_y2 = 0;
			count_x1 = count_x2 = count_y1 = count_y2 = 0;
			for (int i = 0; i < lines.size(); i++) {
				Vec4i l = lines[i];

				x = l[2] - l[0];
				y = l[3] - l[1];

				if (x == 0) {
					slope = 100.0f;
				}
				else {
					slope = (-1)*(y / float(x));
				}

				if (-1.50f < slope and slope < -0.55f) {
					sum_x1 += l[0];
					sum_y1 += l[1];
					sum_x2 += l[2];
					sum_y2 += l[3];
					count_x1 += 1;
					count_x2 += 1;
					count_y1 += 1;
					count_y2 += 1;
				}

			}

			// Line merging
			x1 = sum_x1 / count_x1;
			y1 = sum_y1 / count_y1;
			x2 = sum_x2 / count_x2;
			y2 = sum_y2 / count_y2;



			// Store to draw Imaginary_right_line
			right_temp_x1 = x1;
			right_temp_y1 = y1;
			right_temp_x2 = x2;
			right_temp_y2 = y2;
			//


			
			// if right_line is detected
			if (x2 >= 485 and x2 <= 600)
			{				
				draw_right = true;

				// Draw right_line
				line(result, Point(x1, y1), Point(x2, y2), Scalar(0, 255, 0), 3, 8);

				// Store lower-right point in rightCur_x to find center point
				rightCur_x = x2;
			}
			else
			{
				draw_right = false;
				rightCur_x = rightPrev_x;  // if right_line is not detected, store previous x2 in rightCur_x				
			}

			right_detection = lines.size();



			// Draw center_line //
			edge.copyTo(Center_roi, bgCenter);
			HoughLinesP(Center_roi, lines, 1, CV_PI / 180, 52, 20, 170);

			// Calculate slope of the line
			sum_x1 = sum_x2 = sum_y1 = sum_y2 = 0;
			count_x1 = count_x2 = count_y1 = count_y2 = 0;
			for (int i = 0; i < lines.size(); i++) {
				Vec4i l = lines[i];

				x = l[2] - l[0];
				y = l[3] - l[1];

				if (x == 0) {
					slope = 100.0f;
				}
				else {
					slope = (-1)*(y / float(x));
				}

				if ((2.f < slope) or (-2.f > slope)) {
					sum_x1 += l[0];
					sum_y1 += l[1];
					sum_x2 += l[2];
					sum_y2 += l[3];
					count_x1 += 1;
					count_x2 += 1;
					count_y1 += 1;
					count_y2 += 1;
				}
			}

			// Line merging
			x1 = sum_x1 / count_x1;
			y1 = sum_y1 / count_y1;
			x2 = sum_x2 / count_x2;
			y2 = sum_y2 / count_y2;

			
			// if center_line is detected
			if (x2 > 0)
			{				
				draw_center = true;

				// Draw center_line
				line(result, Point(x1, y1), Point(x2, y2), Scalar(50, 255, 0), 3, 8);

				// Store lower point in centerCur_x to find center point
				centerCur_x = x2;
			}
			else
			{
				draw_center = false;
				centerCur_x = centerPrev_x;  // if center_line is not detected, store previous x2 in centerCur_x				
			}

			center_detection = lines.size();



			// Draw Imaginary_line //

			// Draw Imaginary_right_lane
			if (draw_center == false and draw_right == false and draw_left == true)
			{
				count_left_line += 1;
			}
			else
			{
				count_left_line = 0;
				Imaginary_right_line_flag = false;
			}
			if (count_left_line >= 40)
			{
				if (draw_right == false)
				{
					line(result, Point(left_temp_x1 + 300, left_temp_y1), Point(left_temp_x2 + 150, left_temp_y2), Scalar(255, 0, 0), 3, 8);
					Imaginary_right_line_flag = true;
				}
			}

			// Draw Imaginary_left_line
			if (draw_center == false and draw_right == true and draw_left == false)
			{
				count_right_line += 1;
			}
			else
			{
				count_right_line = 0;
				Imaginary_left_line_flag = false;
			}
			if (count_right_line >= 5)
			{
				if (draw_left == false)
				{
					line(result, Point(right_temp_x1 - 180, right_temp_y1), Point(right_temp_x2 - 320, right_temp_y2), Scalar(255, 0, 0), 3, 8);
					Imaginary_left_line_flag = true;
				}
			}



			
			// Lane Departure System //

			// Find Center point
			int temp_right = rightCur_x;
			int temp_left = leftCur_x;			

			if (draw_right == true  or  draw_left == true  and  (draw_right == false  or  draw_left == false))
			{

				// when car move to the left_line
				if (draw_center == true  and  draw_right == false  and  centerCur_x >= 375  and  centerCur_x <= 475)
				{
					rightCur_x = centerCur_x;
				}
				else
				{
					rightCur_x = temp_right;
				}

				// when car move to the right_line
				if (draw_center == true  and  draw_left == false  and  centerCur_x >= 375  and  centerCur_x <= 475)
				{
					leftCur_x = centerCur_x;
				}
				else
				{
					leftCur_x = temp_left;
				}

			}

			if (Imaginary_right_line_flag == true)
			{
				center_point = (left_temp_x1 + 320 + leftCur_x) / 2;
				rightCur_x = left_temp_x1 + 320;
			}
			else if (Imaginary_left_line_flag == true)
			{
				center_point = (right_temp_x2 - 320 + rightCur_x) / 2;
				leftCur_x = right_temp_x2 - 320;
			}
			else
			{
				center_point = (rightCur_x + leftCur_x) / 2;
			}

			// Draw center_circle using center point
			if (center_point != 0)
			{
				circle(result, Point(center_point, 350), 2, Scalar(0, 0, 255), 2);
			}

			// Range of detecting lane departure 
			rectangle(result, Point(400, 300), Point(445, 390), Scalar(255, 0, 255), 1);

			if (center_point > 400  and  center_point < 445)
			{
				safe_count1 += 1;
			}
			else
			{
				warning_count1 += 1;
			}

			if (safe_count1 >= 10)
			{
				safe_count1 = 0;
				safe_count2 += 1;
			}
			else if (warning_count1 >= 5)
			{
				warning_count1 = 0;
				warning_count2 += 1;
			}

			if (safe_count2 > 3)
			{
				alarm_flag = true;
				safe_count2 = 0;
				warning_count2 = 0;
				warning_count1 = 0;

			}
			else if (warning_count2 > 2)
			{
				alarm_flag = false;
				warning_count2 = 0;
				safe_count2 = 0;
				safe_count1 = 0;
			}

			if (alarm_flag == true)
			{
				putText(result, "", Point(50, 80), FONT_HERSHEY_SIMPLEX, 3, Scalar(0, 255, 0), 3);
			}
			else
			{
				putText(result, "warning", Point(50, 80), FONT_HERSHEY_SIMPLEX, 3, Scalar(0, 0, 255), 3);
			}

			
			// Maintain current value of 'x' until new x value is detected ( = new line is detected )
			leftPrev_x = leftCur_x;
			rightPrev_x = rightCur_x;
			centerPrev_x = centerCur_x;



			imshow("Lane Departure System", result);
			waitKey(1000 / fps);
		}
	}


	waitKey(0);
	return 0;
}