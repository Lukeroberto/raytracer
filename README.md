Raytracer written in c. 

Taken from https://raytracing.github.io/books/RayTracingInOneWeekend.html



## Early work from the course:

Diffuse sphere with lambertian scattering: ![diffuse](examples/diffuse.png)

3 spheres: a diffuse one in the center and two metallic ones (reflective-based scattering) on each side: ![diffuse and metallic](examples/metal_and_diffuse.png)

3 spheres: glass (hollow glass sphere), diffuse, and metal: ![glass](examples/glass.png)

3 spheres adjusting the camera to view from a different angle: ![angle](examples/camera_basis_vectors.png)

3 spheres adjusting field of view for a zoom effect: ![fov](examples/camera_zoom.png)

Final Render shown on the course homepage: ![final](examples/final_render.png)


## Word on getting things realtime
Final render of course homepage, frame-by-frame realtime (settings turned way down): 

![realtime-ish](examples/render.gif)


This is using SDL2 to create the window and render the image to the frame. Everything else is still custom. Next steps are to implement bounding-volume hierarchies to remove the unnecessary ray bounces being calculated all over the scene. Then adding image smoothing and keyboard inputs after that.
