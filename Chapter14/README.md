# Chapter 14

The animation system implemented in earlier chapters of the book utilizes linear blend skinning, or LBS. LBS has some artifacts around where bones bend, as a linear average doesn't produce visually accurate results. Dual quaternions solve this problem, in this chapter you will learn how to implement dual quaternion skinning.