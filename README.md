# COSY Lab IoT Box
## Short Description
This project is part of my bachelor thesis at the University of Vienna. In collaboration with the Cooperated Systems (CoSy) department I'll be building a simple, easily deployable and open (FLOSS) IoT device that students and academics can use for experiments. 

## Goal
The goal is a modular IoT Box that can be configured over the Internet. The Server Client manages a Cluster of Boxes and updates their software and use cases according to the users intent. To achieve that the existing [Seattle Testbed][Seattle Testbed] codebase will be used. This should solve issues like "cluster management", software updates and code pushing.

## Ideas
During the orientation phase, we came up with several practical ideas and scenarios.

| Short description | Measurments | Sensors | Evaluation | Locations | Literature |
|--------------------------------------------------------------------------------------|-----------------------------------------------|---------|------------------------------------------------------------------------------|------------------------------------------------------------|------------|
| Analyze environment in different heights/places | TEMP, HUM, Sound Pollut, CO2 Pollut, PRS, Gas |  | Changes over time, Comparison to other available data and to standard values |  |  |
| Monitor all rooms in an apartement/house | TEMP, HUM, Gas, Light |  | Changes over time, standard values, user centric eval |  |  |
| Register and analyze movements of a skyscraper (could be combined with environments) | Movement | GYRO | Compare to values that should be normal and safe for such high buildings | In-/Outside of a skyscraper in different heights |  |
| Seismographic activities | Sensitive Movement | ACCLRM | Compare  | Outside distributed over a larger area (Vienna or Austria) |  |
| Calculate speed, direction, height, ... of a plane by triangulation | Sound | MIC |  |  |  |
| Monitoring of plants/grass | MOIST, TEMP, Light |  |  | Garden |  |

**These are all estimated guesses for now. It will be updated as this project is developing*

*Key: TEMP: Temperature, HUM: Humidity, Pollut: Pollution, PRS: Pressure, GYRO: Gyrometer, ACCLRM: Accelerometer, MIC: Microphone, MOIST: Moisture*

## Time Table (V2) - [Google Doc][Time Table]

| Task | Title                       | Description                                                                                           | Planned               | Actual Date |
|------|-----------------------------|-------------------------------------------------------------------------------------------------------|-----------------------|-------------|
| 1    | Kick Off                    | Bachelor Seminar                                                                                      | 09.10.2017            | 09.10.2017  |
| 2    | Set Requirements            | Decide and discuss requirements for the thesis                                                        | 12.10.2017            | 12.10.2017  |
| 3    | Set Topic                   | Decide on a topic for the thesis                                                                      | 16.10.2017            | 16.10.2017  |
| 4    | Propose Thesis (MS 1)       | Sign and agree on thesis details                                                                      | 31.10.2017            |             |
| 5    | Design and Setup IoT Box    | Design, build and configure a prototype of the IoT Boxes                                              | 27.10.2017            |             |
| 6    | Construct Software Design   | Design the software using UML (class, component, deployment, object, use case, sequence, ... diagram) | 07.11.2017            |             |
| 7    | Implement design            | Build a first prototype based on the software design and deploy it on the IoT Box                     | 29.11.2017            |             |
| 8    | Review Prototype (MS 2)     | Review changes and development made as well as first prototype                                        | Early Dec. (TBA)      |             |
| 9    | Finalize IoT Box            | Finish Software and implement improvements and further ideas that came out of the review process      | 07.01.2018            |             |
| 10   | Review Final Product (MS 3) | Review changes and development made as well as final product                                          | Mid Jan. (TBA)        |             |
| 11   | Review Thesis               | Submit a first draft of the bachelor thesis for review                                                | 17.02.2018            |             |
| 12   | Present Thesis and Project  | Work in changes into thesis and project before presenting the final product to an audience            | Before Deadline (TBA) |             |
| 13   | Finish Product and Thesis   | Finishand submit product and printed thesis to COSY Lab (Client) before deadline (28.02.2018)         | 26.02.2018            |             |
| XX   | DEADLINE                    |                                                                                                       | 28.02.2018            |             |


*README.MD last updated on 30.10.2017*

[Seattle Testbed]: https://github.com/SeattleTestbed
[Time Table]: https://docs.google.com/spreadsheets/d/1Tz4BFpbCoL11NG1uZ-MAb3D2p4eTi0hClQJtkL02oKs/edit?usp=sharing
