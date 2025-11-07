<h1 align="left">List (DREC TASK)</h1>

###

<div>
  <img style="100%" src="https://capsule-render.vercel.app/api?type=waving&height=90&section=header&reversal=false&fontSize=70&fontColor=FFFFFF&fontAlign=50&fontAlignY=50&stroke=-&descSize=20&descAlign=50&descAlignY=50&theme=cobalt"  />
</div>

###

![til](readme_application/preview.png )

###

This library provides you access to fast and safe data struct named "list".

###

<h2 align="left">Installation and setup guide</h2>

###
```bash
git clone https://github.com/priusf0x/List

```

Then put all files in your project.

###

<h2 align="left">Prerequisites</h2>

###
* `git`
* `make`
* `g++`

Program was tested  and build on specs:
```
Operating System: Manjaro Linux
          Kernel: Linux 6.12.41-1-MANJARO
    Architecture: x86-64
 Hardware Vendor: ASUSTeK COMPUTER INC.
  Hardware Model: ASUS TUF Gaming F15 FX507ZC4_FX507ZC4
Firmware Version: FX507ZC4.312
```


<h2 align="left">Code example</h2>

###

```bash
list_s* list = NULL;

InitList(&list, 2);
ListAddElement(list, 12);
ListAddAfterElement(list, 69, 0);
ListAddElement(list, 12);
ListAddElement(list, 11);
ListAddElement(list, 25);
ListAddElement(list, 32);
ListAddElement(list, 2);
ListDeleteElement(list, 5);
ListDeleteElement(list, 1);

DestroyList(&list);

```




