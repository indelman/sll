function [inliers] = localize(image_a_,image_b_)

im1 = imread(image_a_);
im2 = imread(image_b_);

% make single
im1 = im2single(im1) ;
im2 = im2single(im2) ;

imshow(im2);
input(' ');
%dh1 = max(size(im2,1)-size(im1,1),0) ;
%dh2 = max(size(im1,1)-size(im2,1),0) ;
%imagesc([padarray(im1,dh1,'post') padarray(im2,dh2,'post')]) ;

%imshow(image)
% make grayscale
if size(im1,3) > 1, im1g = rgb2gray(im1) ; else im1g = im1 ; end
if size(im2,3) > 1, im2g = rgb2gray(im2) ; else im2g = im2 ; end

% --------------------------------------------------------------------
%                                                         SIFT matches
% --------------------------------------------------------------------

[f1,d1] = vl_sift(im1g) ;
[f2,d2] = vl_sift(im2g) ;

[matches, scores] = vl_ubcmatch(d1,d2) ;

numMatches = size(matches,2);

X1 = f1(1:2,matches(1,:)); %X1(3,:) = 1 ;
X2 = f2(1:2,matches(2,:)); %X2(3,:) = 1 ;


%---------------------------------------------------------------------
%                                       Estimate Fundamental Matrix using
%                                       RANSAC
%--------------------------------------------------------------------

[F,inliers] = estimateFundamentalMatrix(X1',X2');

%imshow(im1);
end