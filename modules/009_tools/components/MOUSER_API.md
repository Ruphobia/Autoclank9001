# Mouser Search API — integration notes

Reference for the future component-lookup tool that will live in this folder.
The user's API key is read from `settings/credentials.json` -> `mouser.api_key`
(populated via the web UI: **Settings → API Credentials…**). `settings/` is in
`.gitignore`; never commit credentials.

## Endpoint

```
POST https://api.mouser.com/api/v1/search/keyword?apiKey=<KEY>
Content-Type: application/json
Accept: application/json
```

The API key is passed as a **query parameter** on the URL, not a header.

## Request body (search by keyword)

```json
{
  "SearchByKeywordRequest": {
    "keyword": "ATmega328P",
    "records": 10,
    "startingRecord": 0,
    "searchOptions": "",
    "searchWithYourSignUpLanguage": ""
  }
}
```

- `keyword`: free-text search (manufacturer part number, description, etc.)
- `records`: 1–50 results per call
- `startingRecord`: pagination offset
- `searchOptions`: `"Rohs"`, `"InStock"`, `"RohsAndInStock"`, or empty
- `searchWithYourSignUpLanguage`: language hint (usually leave empty)

## Other endpoints worth knowing

- `POST /api/v1/search/partnumber` — exact part number lookup
- `POST /api/v1/search/manufacturerlist` — list all manufacturers
- `POST /api/v1/orderhistory/byDate` — order history (account-bound)
- `POST /api/v1/order/<orderNumber>` — order details

## Response shape (excerpt)

```json
{
  "Errors": [],
  "SearchResults": {
    "NumberOfResult": 42,
    "Parts": [
      {
        "ManufacturerPartNumber": "ATMEGA328P-PU",
        "Manufacturer": "Microchip Technology",
        "Description": "8-bit Microcontrollers - MCU AVR 20MHz 32kB Flash...",
        "DataSheetUrl": "https://www.mouser.com/datasheet/2/268/...pdf",
        "MouserPartNumber": "556-ATMEGA328P-PU",
        "ProductDetailUrl": "https://www.mouser.com/...",
        "AvailabilityInStock": "1234",
        "PriceBreaks": [{ "Quantity": 1, "Price": "$2.34", "Currency": "USD" }],
        "Min": "1",
        "Mult": "1",
        "Category": "Embedded - Microcontrollers"
      }
    ]
  }
}
```

## Rate limits

Mouser publishes no hard rate limit publicly; their docs say "fair use".
Conservative client behavior: 1 request/second, exponential backoff on 429.

## Free key signup

https://www.mouser.com/api-hub/ — create a Mouser account, request a Search
API key (instant), paste into our **Settings → API Credentials…** panel.
