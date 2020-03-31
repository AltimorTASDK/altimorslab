float nlRecipSqrt(float value);

float sqrtf(float value)
{
	if (value == 0)
		return 0;

	return 1 / nlRecipSqrt(value);
}