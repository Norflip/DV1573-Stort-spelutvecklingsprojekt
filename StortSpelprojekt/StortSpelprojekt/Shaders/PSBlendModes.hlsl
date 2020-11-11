

float GetL(float3 color)
{
    float RedGreenBlue[3];

    RedGreenBlue[0] = color.r;
    RedGreenBlue[1] = color.g;
    RedGreenBlue[2] = color.b;
    float smallest = 9999999;
    float biggest = 0;

    for (int i = 0; i < 3; i++)
    {
        if (RedGreenBlue[i] < smallest)
        {
            smallest = RedGreenBlue[i];
        }
        if (RedGreenBlue[i] > biggest)
        {
            biggest = RedGreenBlue[i];
        }
    }

    return (smallest + biggest) / 2.0f;
}



float GetS(float3 color)
{
    float RedGreenBlue[3];

    RedGreenBlue[0] = color.r;
    RedGreenBlue[1] = color.g;
    RedGreenBlue[2] = color.b;

    float smallest = 999999999;
    float biggest = 0;

    for (int i = 0; i < 3; i++)
    {
        if (RedGreenBlue[i] < smallest)
        {
            smallest = RedGreenBlue[i];
        }
        if (RedGreenBlue[i] > biggest)
        {
            biggest = RedGreenBlue[i];
        }
    }

    if (smallest == biggest)
    {
        return 0.0;
    }
    else
    {
        if (GetL(color) < 0.5)
        {
            return (biggest - smallest) / (biggest + smallest);
        }
        else if (GetL(color) >= 0.5)
        {
            return (biggest - smallest) / (2.0f - biggest - smallest);
        }
        else
        {
            return 0.0f; //Added this so that it will always return something.
        }
    }
}


float GetH(float3 color)
{

    float RedGreenBlue[3];

    RedGreenBlue[0] = color.r;
    RedGreenBlue[1] = color.g;
    RedGreenBlue[2] = color.b;
    float smallest = 99999999;
    float biggest = 0;
    float hue = 0.0;
    for (int i = 0; i < 3; i++)
    {
        if (RedGreenBlue[i] < smallest)
        {
            smallest = RedGreenBlue[i];
        }
        if (RedGreenBlue[i] > biggest)
        {
            biggest = RedGreenBlue[i];
        }
    }

    if (smallest == biggest)
    {

        return hue;
    }
    else
    {
        if (biggest == RedGreenBlue[0])
        {
            hue = (RedGreenBlue[1] - RedGreenBlue[2]) / (biggest - smallest);
        }
        else if (biggest == RedGreenBlue[1])
        {
            hue = 2 + (RedGreenBlue[2] - RedGreenBlue[0]) / (biggest - smallest);
        }
        else if (biggest == RedGreenBlue[2])
        {
            hue = 4 + (RedGreenBlue[0] - RedGreenBlue[1]) / (biggest - smallest);
        }

        hue *= 60;

        if (hue < 0)
        {
            hue += 360;
        }
    }

    return hue;
}


float3 PSColorMode(float3 color, float h, float s, float lum, float opacity)
{
    float3 resultRGB = float3(0, 0, 0);

    int counter = 0;
    float resultF = 0;
    float temp1 = 0;
    float temp2 = 0;
    float3 tempRGB;
    tempRGB.r = 0;
    tempRGB.g = 0;
    tempRGB.b = 0;
    if (s == 0.0)
    {
        resultRGB.r = lum;
        resultRGB.g = lum;
        resultRGB.b = lum;
    }
    else
    {
        if (lum < 0.5)
        {
            temp1 = lum * (1 + s);
        }
        else if (lum >= 0.5)
        {
            temp1 = (lum + s) - (lum * s);

        }

        temp2 = (2 * lum) - temp1;

        h /= 360.0f;

        tempRGB.r = (float)(h + 0.333);
        tempRGB.g = h;
        tempRGB.b = (float)(h - 0.333);

        if (tempRGB.r < 0)
        {
            tempRGB.r += 1;
        }
        else if (tempRGB.r > 1)
        {
            tempRGB.r -= 1;
        }
        if (tempRGB.g < 0)
        {
            tempRGB.g += 1;
        }
        else if (tempRGB.g > 1)
        {
            tempRGB.g -= 1;
        }
        if (tempRGB.b < 0)
        {
            tempRGB.b += 1;
        }
        else if (tempRGB.b > 1)
        {
            tempRGB.b -= 1;
        }

        if (6 * tempRGB.r < 1)
        {
            resultF = (temp2 + (temp1 - temp2)) * 6 * tempRGB.r;
        }
        else if (2 * tempRGB.r < 1)
        {
            resultF = temp1;
        }
        else if (3 * tempRGB.r < 2)
        {
            resultF = (temp2 + (temp1 - temp2)) * (float)((0.666 - tempRGB.r)) * 6;
        }
        else
        {
            resultF = temp2;
        }

        resultRGB.r = resultF;
        if (6 * tempRGB.g < 1)
        {
            resultF = (temp2 + (temp1 - temp2)) * 6 * tempRGB.g;
        }
        else if (2 * tempRGB.g < 1)
        {
            resultF = temp1;
        }
        else if (3 * tempRGB.g < 2)
        {
            resultF = (temp2 + (temp1 - temp2)) * (float)((0.666 - tempRGB.g)) * 6;
        }
        else
        {
            resultF = temp2;
        }

        resultRGB.g = resultF;
        if (6 * tempRGB.b < 1)
        {
            resultF = (temp2 + (temp1 - temp2)) * 6 * tempRGB.b;
        }
        else if (2 * tempRGB.b < 1)
        {
            resultF = temp1;
        }
        else if (3 * tempRGB.b < 2)
        {
            resultF = (temp2 + (temp1 - temp2)) * (float)((0.666 - tempRGB.b)) * 6;
        }
        else
        {
            resultF = temp2;
        }

        resultRGB.b = resultF;
    }

    resultRGB.r = ((1 - opacity) * color.r + (opacity * resultRGB.r));
    resultRGB.g = ((1 - opacity) * color.g + (opacity * resultRGB.g));
    resultRGB.b = ((1 - opacity) * color.b + (opacity * resultRGB.b));

    return resultRGB;
}




float3 Overlay(float3 a, float3 b)
{
    float3 grey = float3(0.5, 0.5, 0.5);
    float3 white = float3(1, 1, 1);
    float3 result = float3(0,0,0);

    if (b.r+b.g+b.b <= grey.r+grey.g+grey.b)
    {
        result = 2 * a * b;
    }
    else if (b.r + b.g + b.b > grey.r + grey.g + grey.b)
    {
        result = white - (2 * (white - a) * (white - b));
    }

    return result;
}


float3 Screen(float3 a, float3 b)
{
    float3 white = float3(1, 1, 1);
    return white - (white - a) * (white - b);
}


float3 LowerOpacity(float3 a, float3 b, float scalar)
{
    b.r = ((1 - scalar) * a.r + (scalar * b.r));
    b.g = ((1 - scalar) * a.g + (scalar * b.g));
    b.b = ((1 - scalar) * a.b + (scalar * b.b));

    return b;
}