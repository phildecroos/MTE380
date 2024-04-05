float clamp(float val, float min, float max)
{
  if (val > max)
    return max;
  else if (val < min)
    return min;
  return val;
}

void shift(bool* arr, int len)
{
  for (int i = 1; i < len; i++)
  {
    arr[len - i] = arr[len - i - 1];
  }
}

bool check_all(bool* arr, int len)
{
  bool stop = true;
  for (int i = 0; i < len; i++)
  {
    if (!arr[i])
      stop = false;
  }
  return stop;
}