# Adding anisotropy control to the Micro-meshes schema
![Static Badge](https://img.shields.io/badge/build-passing-green)
![Static Badge](https://img.shields.io/badge/license-MIT-yellowgreen)
![Static Badge](https://img.shields.io/badge/language-C%2B%2B-brightgreen)
[![Ask Me Anything !](https://img.shields.io/badge/Ask%20me-anything-1abc9c.svg)](mailto:pagliuca.manuel@gmail.com)

## About the project
Final project for the master's degree in Computer Science A.A. 2022/2023.

### Abstract
This thesis aims to empirically investigate the imaginable performance for data structures suitable for effectively representing extreme-resolution 3D polygonal surfaces designed for multi-resolution rendering on GPUs.

To this end, supporting algorithms will be designed, implemented, and tested that transform "traditional" (i.e., indexed) high-resolution triangular meshes into the analyzed data structures, and then measure the approximation errors introduced through appropriate geometric measurements.

Other alternative schemes will be studied, which are considered variants of the so-called "micro-meshes" scheme offered by the latest generation of vendor-specific GPU hardware. These data structures are characterized by the use of a semi-regular subdivision of a medium-resolution "basic mesh," followed by displacement of the generated vertices. Variants introduced may include the adoption of an anisotropic subdivision step, the adoption of an irregular recursive subdivision scheme, or others.

## Settings
Compiler:
* aggiungere nella PATH il compilatore utilizzato di Qt (per l'esecuzione dello script)

## Dependencies
* Python 3.11
  * PyMeshLab
* glm

## Models
Models that I've used for the empirical analysis:
* models pallas cat-> author
* [Orc dude sketch free 3D print model](https://www.cgtrader.com/free-3d-print-models/art/sculptures/orc-bust-d7a81f2f-6e47-45a7-88ce-108ea008be67) by FRKN
* dragon -> https://www.cgtrader.com/free-3d-models/animals/reptile/dragon-f03649c0-cf42-4c8b-9edc-849703314a7b

## Python script (empirical analysis)
TODO: set the x number of samples to use

Executing this script will generate **x** samples for both the subdivision schemes (classic and anisotropic). A table (as a text file) containing the Hausdorff distances toward the target mesh will be built for both batches of samples.

The table will be ordered by the number of micro-faces used, this will allow the comparison of one sample of a table with a sample of the other table, using the number of micro-faces as the comparative criteria.
- As an example, with the two given tables it is possible to compare the distance for the **micro-mesh** sample of 10,000 micro-faces will be tested against the 10,000 **anisotropic micro-mesh** sample

TODO: Add thesis link
In the thesis, the comparison is described by the scatter plot of pag. **x** (logarithmic scale on y-axis).

The commands generate two batches of samples, multiple execution of the commands are used for the analysis since multiple models are tested.
```batch
python compute-distances.py --base-mesh=model.obj --target-mesh=target.obj
```
Default values if you *omit* some of the options:
* `--base-mesh = pallas_124.obj`
* `--target-mesh = pallas_5000.obj`
* `--min-edge = 0.1`
* `--max-edge = 10.0`

## Application
### Graphical User Interface

TODO: some gifs

### Commands for the CLI

```cmd
{o.exe} build-samples n=7 min_val=0.3 max_val=7.0 target_faces=5000
```

```cmd
anisotropic_micromesh.exe build-samples --base-mesh=pallas_125.obj --target=pallas_5000.obj --n=4 --min-edge=0.8 --max-edge=1.2
```
