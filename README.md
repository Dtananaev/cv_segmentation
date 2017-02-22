Computer vision: segmentation
====================================================



[![Build Status](https://travis-ci.org/Dtananaev/k_means.svg?branch=master)](https://travis-ci.org/Dtananaev/k_means)
[![BSD2 License](http://img.shields.io/badge/license-BSD2-brightgreen.svg)](https://github.com/Dtananaev/k_means/blob/master/LICENSE.md) 

* k_means - k-means segmentation 
<p align="center">
  <img src="https://github.com/Dtananaev/k_means/blob/master/pictures/tesla.jpg" width="350"/>
  <img src="https://github.com/Dtananaev/k_means/blob/master/pictures/tesla_clustered.jpg" width="350"/>
</p>
     * To install use in terminal: 
         * cd k_means
         * make
     * To run: ./kmeans name_of_file(without .ppm) \<number of clusters\> \<number of iteration\>
     
     

* two region segmentation based on variatinal methods allows to segment noisy images where the k-means segmentation ussualy fail because it adds noise to one of the segment. 2region segmentation approach could be easily extended to multi region segmentation
<p align="center">
  <img src="https://github.com/Dtananaev/cv_segmentation/blob/master/pictures/withWholeEasier.jpg" width="350"/>
  <img src="https://github.com/Dtananaev/cv_segmentation/blob/master/pictures/withWholeEasier_2r.jpg" width="350"/>
</p>
     * To install use in terminal: 
         * cd two_region_segmentation
         * make
     * To run: ./segment2r name_of_file(without .pgm)
