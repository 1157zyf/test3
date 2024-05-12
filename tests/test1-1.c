// 可追加putint函数的原型，避免静态检查或编译警告的产生
// 如果不追加原型，编译时需通过--include选型指定头文件忽略警告

int g = 3;
int main()
{
    // int b, c = 5, d = g + 3 * 2;
    // int a = b + 3;

    // int s[2][3], a[10];

    int a = 0;
    if (a == 0) {
        putint(1);
    } else
        putint(2);

    while (1) {
        int b = 0;
        b = b * 3;
    }
    return 0;
}
