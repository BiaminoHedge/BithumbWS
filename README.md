Реализовать многопоточную отправку запросов на https://feed.bithumb.com/_next/data/HMaq2yVQy0nQtliwaAqW6/notice.json?category=9&page=1 (веб https://feed.bithumb.com/notice?category=9&page=1). Парсинг данных и отправку символа по вебсокету.

## Детально
1) Посылаем параллельно запросы в максимально доступное количество потоков на https://feed.bithumb.com/_next/data/HMaq2yVQy0nQtliwaAqW6/notice.json?category=9&page=1.
Получаем json

{"pageProps":{"status":"ok","noticeList":[{"id":1649738,"boardType":"1","categoryName1":"마켓 추가","categoryName2":null,"title":"월드 리버티 파이낸셜 유에스디(USD1) 원화 마켓 추가","topFixYn":"N","publicationDateTime":"2025-09-01 13:35:11","modifyDateTime":"2025-09-02 11:49:04","modifyDateTimeExposureYn":"N"}...

2) Извлекаем токены только по форме (TOKEN).
    Считаем токеном всё, что в круглых скобках и написано латиницей/цифрами/._-.
    Пример:
        "캠프 네트워크(CAMP) 원화 마켓 추가" -> ["CAMP"]
        "휴머니티 프로토콜(H), 만트라(OM)" -> ["H", "OM"]
        "월드 리버티 파이낸셜 유에스디(USD1) 원화 마켓 추가" -> ["USD1"]

3) Отсылаем сообщение по вебсокету на подключенные клиенты.

Важно, чтобы все работало параллельно, запросы не падали, как и вебсокет
