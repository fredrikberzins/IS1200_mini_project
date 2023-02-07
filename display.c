// Display translation file for chipKIT Uno32 board with a I2C screen
// x=0 and y=0 is in the lower left corner
// Pong specilaced global functions
//     move_paddle(paddle_sel[bool(1 left, 0 right)], new_pos[int(0-55)]),
//     move_ball(new_x[int(0-127)], new_y[int(0-63)], old_x[int(0-127)], old_y[int(0-63)]),
//     render_points(left_player[int], right_player[int])
// Pong private functions
//     change_solid(color[bool(0 black, 1 white)], x1[int(0-127)], y1[int(0-63)], x2[int(0-127)], y2[int(0-63)])
//     change_score(pos, num)//not started scetch


// Genaral plan
//change_solid(color, x1, y1, x2, y2)
//  if (!color)
//      print black over the whole area
//  else()
//      print white over the whole area
//  if(x1-64*x2-63 > 0)
//  update screan

//move_paddle(paddle_sel, new_pos)
//  change_solid(False,(9+paddle*108),0,(9+paddle*108),63) // paddle place (9+paddle*108) offset 9p and a distance between of 108p
//  change_solid(True, pos, (9+paddle*108), pos+8, (10+paddle*108))

//move_ball([new_x, new_y, old_x, old_y])
//  change_solid(False, old_x, old_y, old_x+1, old_y+1)
//  change_solid(True, new_x, new_y, old_x+1, new_y+1)

//render_points(point[left_player, right_player])
//for(int i=0; i<2; i++)
//  for (amount of numbers in points[i])
//      if (i == 0)
//          number_cord = 64 + 2 + width * number_pos
//      if (i == 1)
//          number_cord = 63 - 2 - width * number-number_pos
//      delect number in list and add to wwrite array