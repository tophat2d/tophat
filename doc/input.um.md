# umka/input.um

```
 vim: filetype=umka
```

## fn setwinmap*
`fn setwinmap*()`



## fn getmousepos*
`fn getmousepos*(): (int32, int32)`

Returns the position of mouse cursor
in relation to the screen.


## fn getglobalmousepos*
`fn getglobalmousepos*(cam: rectangle.rect): (int32, int32)`

Returns the position of mouse cursor
in relation to cam.


## fn ispressed*
`fn ispressed*(code: int32): bool`

Returns if key is pressed. Either use
codes defined in the file above, or
pass lower case char/number.


## fn isjustpressed*
`fn isjustpressed*(code: int32): bool`

Returns, if code was just pressed
this loop.



