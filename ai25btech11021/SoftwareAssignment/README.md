This is about the overview of this Project 


I provided 

1.Aim
2.Outcomes
3.Extending to RGB colours
4.Summary of Strang's Video
5.Implemented Algorithm
6.Comparing Different Algorithms
7.Reconstructed Images
8.Error Analysis
9.Discussion between k, image quality, and compression
10.Discussion of trade-offs and reflections on implementation choice
11.Visualization of eigen values



The main goal of this project was to use SVD for image compression. The idea is to keep only the most important singular values (the top k) to create a smaller, compressed version of the image, while throwing away the less important ones that often just represent noise.

I worked with both grayscale and colour images. For colour images, I applied the SVD process separately to each of the three colour channels (Red, Green, and Blue) to handle the compression properly. I wrote a C program that can handle both types of images.

I implemented a specific algorithm called the Randomized Power Iteration method for SVD. I chose this method because it's much faster and more efficient for large images compared to a full SVD, especially when you only need a few dominant singular values. It works by using random sampling to approximate the most important parts of the image matrix.

I tested the compression with different values of k (like 2, 20, 50, 100, 200) on several images. For each one, I calculated the Frobenius error to measure how much information was lost. As expected, a lower k gives a more compressed but blurrier image, while a higher k gives a clearer image but with a larger file size.

There's a clear trade-off between image quality and compression. You have to choose a k value that balances a small file size with acceptable visual quality. The results show that you don't need a very high k to get a decent-looking image, which proves the effectiveness of SVD compression.

Overall, this project was a good practical way to understand how SVD works and how linear algebra concepts are used in real-world applications like image compression. It also helped me compare different computational methods and their efficiencies.

This provides the whole overview of the Project.