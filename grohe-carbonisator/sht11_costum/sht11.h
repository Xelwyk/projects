/*
 * Initialize.
 */
void		sht11_init(void);


/*
 * Start measurement (humidity or temperature).
 * Return "device found".
 * Afterwards poll sht11_ready.
 */
void		sht11_start_temp(void);
void		sht11_start_humid(void);

/*
 * Return 0 unless measurement completed.
 */
uint8_t		sht11_ready(void);

/*
 * Return result of measurement.
 * H: 100*%RH (0..10000)
 * T: 100*T
 * Return -32xxx on failure.
 */
int16_t		sht11_result_temp(void);
int16_t		sht11_result_humid(void);
//double		sht11_result_humid(void);
