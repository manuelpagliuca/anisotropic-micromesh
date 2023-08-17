# project name

* aggiungere nella PATH il compilatore utilizzato di Qt (per l'esecuzione dello script)

# Dependencies
* pip3 install pymeshlab
* glm


* models pallas cat-> author
* model orc -> https://www.cgtrader.com/free-3d-print-models/art/sculptures/orc-bust-d7a81f2f-6e47-45a7-88ce-108ea008be67
* dragon -> https://www.cgtrader.com/free-3d-models/animals/reptile/dragon-f03649c0-cf42-4c8b-9edc-849703314a7b
## Script commands

Generic Python script
```py
python compute-distances.py --base-mesh=model.obj --target-mesh=target.obj --criterion=same-microfaces
```
There are some default values if you omit some of the options:
* `--base-mesh=pallas_124.obj`
* `--target-mesh=pallas_5000.obj`
* `--criterion=same-microfaces`

There is also an optional criteria which uses same target edge length for the comparison between the displaced models.

### Qt App

```cmd
{o.exe} build-samples n=7 min_val=0.3 max_val=7.0 target_faces=5000
```

```cmd
anisotropic_micromesh.exe build-samples --base-mesh=pallas_125.obj --target=pallas_5000.obj --n=4 --min-edge=0.8 --max-edge=1.2
```

### Meshlab