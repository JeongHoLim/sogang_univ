#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "mysql.h"

#pragma comment(lib, "libmysql.lib")

const char* host = "localhost";
const char* user = "root";
const char* pw = "20171682";
const char* db = "project2";

int main(void) {

	MYSQL* connection = NULL;
	MYSQL* tempconnection = NULL;
	MYSQL conn;
	MYSQL_RES* sql_result;
	MYSQL_ROW sql_row;
	int type;
	int go;
	FILE* fp;
	const char query[3000];					// create table
	char* buffer, * temp;
	int size, state = 0, i = 0;
	char command[100];
	char temp2[2000];
	int K;
	int fields;
	if (mysql_init(&conn) == NULL)
		printf("mysql_init() error!");

	connection = mysql_real_connect(&conn, host, user, pw, db, 3306, (const char*)NULL, 0);
	if (connection == NULL)
	{
		printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
		return 1;
	}
	else
	{
		printf("Connection Succeed\n");
		if (mysql_select_db(&conn, db)) {
			printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
			return 1;
		}

		fp = fopen("20171682_1.txt", "r");				//create table & insert values
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		buffer = malloc((size_t)size + 1);
		memset(buffer, 0, (size_t)size + 1);
		fseek(fp, 0, SEEK_SET);
		fread(buffer, size, 1, fp);

		temp = strtok(buffer, ";");
		while (1) {
			if (temp == NULL) break;
			sprintf(query, "%s", temp);
			state = mysql_query(connection, query);
			temp = strtok(NULL, ";");
		}
		fclose(fp);





		while (1) {
			printf("------- SELECT QUERY TYPES -------\n\n");
			printf("\t1. TYPE 1\n");
			printf("\t2. TYPE 2\n");
			printf("\t3. TYPE 3\n");
			printf("\t4. TYPE 4\n");
			printf("\t5. TYPE 5\n");
			printf("\t6. TYPE 6\n");
			printf("\t7. TYPE 7\n");
			printf("\t0. QUIT\n");
			printf("----------------------------------\n");
			printf("\nInput(TYPE) : ");
			scanf("%d", &type);
			getchar();
			if (type == 0) {
				fp = fopen("20171682_2.txt", "r");
				fseek(fp, 0, SEEK_END);
				size = ftell(fp);
				buffer = malloc((double)size + 1);
				memset(buffer, 0, (double)size + 1);
				fseek(fp, 0, SEEK_SET);
				fread(buffer, size, 1, fp);
				temp = strtok(buffer, ";");
				while (1) {
					if (temp == NULL) break;
					sprintf(query, "%s", temp);
					state = mysql_query(connection, query);
					temp = strtok(NULL, ";");
				}
				fclose(fp);
				break;
			}
			else if (type == 1) {
				printf("\n\n");
				printf("---- TYPE 1 ----\n\n");
				printf("** Show the sales trends for a particular brand over the past k years**\n");
				printf(" Which brand? : ");
				fgets(command, 100, stdin);
				command[strlen(command) - 1] = 0;
				printf(" Which K? : ");
				scanf("%d", &K);
				getchar();

				sprintf(temp2, "create view bid as select b_id from brand where name = \"%s\"; select year(purchase_date) as year, count(*) as sales	from( select * from model natural join bid natural join vehicle where year(purchase_date) >= year(date_sub(NOW(), interval %d year))) as S group by year(S.purchase_date)", command, K);

				temp = strtok(temp2, ";");
				printf("year\tsales\n");
				while (1) {
					if (temp == NULL) {
						break;
					}
					sprintf(query, "%s", temp);
					state = mysql_query(connection, query);
					if (state == 0) {
						sql_result = mysql_store_result(connection);
						if (sql_result != NULL) {
							while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
								fields = mysql_num_fields(sql_result);
								for (i = 0; i < fields; i++)
									printf("%s\t", sql_row[i]);
								printf("\n");
							}
						}
					}
					temp = strtok(NULL, ";");
				}
				printf("\n---- Subtypes in TYPE 1 ----\n");
				printf("\t1. TYPE 1-1\n");
				printf("\t2. QUIT\n");
				printf("Input : ");
				scanf("%d", &go);
				getchar();
				if (go == 2) {
					state = mysql_query(connection, "drop view bid");
					memset(temp2, 0, sizeof(temp2));
					printf("\n");
					continue;
				}
				printf("\nThen break these data out by gender of the buyer\n");
				memset(temp2, 0, sizeof(temp2));
				sprintf(temp2, "with findk(c_id,date,gender) as (select T.c_id, purchase_date, gender from model natural join bid natural join vehicle as T, customer as C where year(purchase_date) >= year(date_sub(NOW(), interval %d year)) and C.c_id = T.c_id) select year(date) as year, count(*) as sales, count(case when gender = 'F' then 1 end) as Female, count(case when gender = 'M' then 1 end) as Male from findk group by year(date); ", K);

				temp = strtok(temp2, ";");
				printf("year\tsales\tFemale\tMale\n");
				while (1) {
					if (temp == NULL) {
						break;
					}
					sprintf(query, "%s", temp);
					state = mysql_query(connection, query);
					if (state == 0) {
						sql_result = mysql_store_result(connection);
						if (sql_result != NULL) {
							while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
								fields = mysql_num_fields(sql_result);
								for (i = 0; i < fields; i++)
									printf("%s\t", sql_row[i]);
								printf("\n");
							}
						}
					}
					temp = strtok(NULL, ";");
				}
				printf("\n---- Subtypes in TYPE 1-1 ----\n");
				printf("\t1. TYPE 1-1-1\n");
				printf("\t2. QUIT\n");
				printf("Input : ");
				scanf("%d", &go);
				getchar();
				if (go == 2) {
					state = mysql_query(connection, "drop view bid");
					memset(temp2, 0, sizeof(temp2));
					printf("\n");
					continue;
				}
				printf("\nThen by income range \n");
				memset(temp2, 0, sizeof(temp2));
				sprintf(temp2, "with findk(c_id,date,gender,income) as (select T.c_id, purchase_date, gender, annual_income from model natural join bid natural join vehicle as T, customer as C where year(purchase_date) >= year(date_sub(NOW(), interval %d year)) and C.c_id = T.c_id ) select year(date) as year, count(*) as sales, count(case when gender = 'F' then 1 end) as Female,count(case when gender = 'M' then 1 end) as Male,count(case when income < 100000 and gender = 'F' then 1 end) as 'Female(income < 100000)', count(case when income < 100000 and gender = 'M' then 1 end), count(case when 100000 <= income and income < 150000 and gender = 'F' then 1 end),count(case when 100000 <= income and income < 150000 and gender = 'M' then 1 end) , count(case when 150000 <= income and income < 200000 and gender = 'F' then 1 end) ,count(case when 150000 <= income and income < 200000 and gender = 'M' then 1 end), count(case when income >= 200000 and gender = 'F' then 1 end),count(case when income >= 200000 and gender = 'M' then 1 end) from findk	group by year(date); ", K);
				temp = strtok(temp2, ";");
				printf("year\tsales\tFemale\tMale\tF(I<0.1M$) M(I<0.1M$) F(0.1M$ <= I<0.15M$) M(0.1M$ <=I <0.15M$) F(0.15M$ <=I <0.2M$) M(0.15M$ <=I < 0.2M$) F(I>=0.2M$) M(I>=0.2M$)\n");
				while (1) {
					if (temp == NULL) {
						break;
					}
					sprintf(query, "%s", temp);
					state = mysql_query(connection, query);

					if (state == 0) {
						sql_result = mysql_store_result(connection);
						if (sql_result != NULL) {
							while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
								fields = mysql_num_fields(sql_result);
								for (i = 0; i < fields; i++) {
									if (i == 4 || i == 5) printf("%-11s", sql_row[i]);
									else if (i < 4)  printf("%s\t", sql_row[i]);
									else if (i == 10 || i == 11) printf("%-12s", sql_row[i]);
									else if (i == 9) printf("%-22s", sql_row[i]);
									else printf("%-21s", sql_row[i]);
								}
								printf("\n");
							}
						}
					}
					temp = strtok(NULL, ";");

				}

				state = mysql_query(connection, "drop view bid");
				memset(temp2, 0, sizeof(temp2));
				printf("\n");
			}
			else if (type == 2) {
				printf("\n\n");
				printf("---- TYPE 2 ----\n\n");
				printf("** Show sales trends for various brands over the past k months**\n");
				printf(" Which K? : ");
				scanf("%d", &K);
				getchar();

				sprintf(temp2, "with temp(bname,mid,date) as (select B.name as bname, M.m_id as mid, purchase_date	from model as M, brand as B natural join vehicle as T where M.b_id = B.b_id and M.m_id = T.m_id and T.c_id is not null and date_format(purchase_date,\"%%Y-%%m\") >= date_format(date_sub(NOW(), interval %d month),\"%%Y-%%m\")) select bname, date_format(date, \"%%Y-%%m\") as \"year-month\", count(*) as sales from temp group by bname, month(date) order by date;", K);
				temp = strtok(temp2, ";");
				printf("%-10syyyy-mm\tsales\n", "brand");
				while (1) {
					if (temp == NULL) {
						break;
					}
					sprintf(query, "%s", temp);
					state = mysql_query(connection, query);

					if (state == 0) {
						sql_result = mysql_store_result(connection);
						if (sql_result != NULL) {
							while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
								fields = mysql_num_fields(sql_result);
								for (i = 0; i < fields; i++) {
									if (i == 0) printf("%-10s", sql_row[i]);
									else printf("%s\t", sql_row[i]);
								}
								printf("\n");
							}
						}
					}
					temp = strtok(NULL, ";");
				}
				printf("\n---- Subtypes in TYPE 2 ----\n");
				printf("\t1. TYPE 2-1\n");
				printf("\t2. QUIT\n");
				printf("Input : ");
				scanf("%d", &go);
				getchar();
				if (go == 2) {
					memset(temp2, 0, sizeof(temp2));
					printf("\n");
					continue;
				}
				printf("\nThen break these data out by gender of the buyer\n");
				memset(temp2, 0, sizeof(temp2));
				sprintf(temp2, "with temp(bname,mid,date,gender) as (select B.name as bname, M.m_id as mid, purchase_date, gender from model as M, brand as B natural join vehicle as T, customer as C where M.b_id = B.b_id and M.m_id = T.m_id and T.c_id = C.c_id and date_format(purchase_date,\"%%Y-%%m\") >= date_format(date_sub(NOW(), interval %d month),\"%%Y-%%m\")) select bname, date_format(date, \"%%Y-%%m\") as \"year-month\", count(*) as sales, count(case when gender = 'F' then 1 end) as Female,	count(case when gender = 'M' then 1 end) as Male from temp group by bname, month(date) order by date;", K);
				temp = strtok(temp2, ";");
				printf("%-10syyyy-mm\tsales\tFemale\tMale\n", "brand");
				while (1) {
					if (temp == NULL) {
						break;
					}
					sprintf(query, "%s", temp);
					state = mysql_query(connection, query);

					if (state == 0) {
						sql_result = mysql_store_result(connection);
						if (sql_result != NULL) {
							while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
								fields = mysql_num_fields(sql_result);
								for (i = 0; i < fields; i++) {
									if (i == 0) printf("%-10s", sql_row[i]);
									else printf("%s\t", sql_row[i]);
								}
								printf("\n");
							}
						}
					}
					temp = strtok(NULL, ";");
				}
				printf("\n---- Subtypes in TYPE 2-1 ----\n");
				printf("\t1. TYPE 2-1-1\n");
				printf("\t2. QUIT\n");
				printf("Input : ");
				scanf("%d", &go);
				getchar();
				if (go == 2) {
					memset(temp2, 0, sizeof(temp2));
					printf("\n");
					continue;
				}
				printf("Then by income range \n");
				memset(temp2, 0, sizeof(temp2));
				sprintf(temp2, "with temp(bname,mid,date,gender,income) as (select B.name as bname, M.m_id as mid, purchase_date, gender, annual_income from model as M, brand as B natural join vehicle as T, customer as C where M.b_id = B.b_id and M.m_id = T.m_id and T.c_id = C.c_id and date_format(purchase_date,\"%%Y-%%m\") >= date_format(date_sub(NOW(), interval %d month),\"%%Y-%%m\")) select bname, date_format(date, \"%%Y-%%m\") as \"year-month\", count(*) as sales,count(case when gender = 'F' then 1 end) as Female, count(case when gender = 'M' then 1 end) as Male, count(case when income < 100000 and gender = 'F' then 1 end) as 'Female(income < 100000)', count(case when income < 100000 and gender = 'M' then 1 end), count(case when 100000 <= income and income < 150000 and gender = 'F' then 1 end), count(case when 100000 <= income and income < 150000 and gender = 'M' then 1 end), count(case when 150000 <= income and income < 200000 and gender = 'F' then 1 end), count(case when 150000 <= income and income < 200000 and gender = 'M' then 1 end), count(case when income >= 200000 and gender = 'F' then 1 end), count(case when income >= 200000 and gender = 'M' then 1 end) from temp group by bname, month(date) order by date;", K);
				temp = strtok(temp2, ";");
				printf("%-10syyyy-mm\tsales\tFemale\tMale\tF(I<0.1M$) M(I<0.1M$) F(0.1M$ <= I<0.15M$) M(0.1M$ <=I <0.15M$) F(0.15M$ <=I <0.2M$) M(0.15M$ <=I < 0.2M$) F(I>=0.2M$) M(I>=0.2M$)\n", "brand");
				while (1) {
					if (temp == NULL) {
						break;
					}
					sprintf(query, "%s", temp);
					state = mysql_query(connection, query);

					if (state == 0) {
						sql_result = mysql_store_result(connection);
						if (sql_result != NULL) {
							while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
								fields = mysql_num_fields(sql_result);
								for (i = 0; i < fields; i++) {
									if (i == 0) printf("%-10s", sql_row[i]);
									else if (i == 5 || i == 6) printf("%-11s", sql_row[i]);
									else if (i < 5)  printf("%s\t", sql_row[i]);
									else if (i == 11 || i == 12) printf("%-12s", sql_row[i]);
									else if (i == 10) printf("%-22s", sql_row[i]);
									else printf("%-21s", sql_row[i]);
								}
								printf("\n");
							}
						}
					}
					temp = strtok(NULL, ";");
				}
				memset(temp2, 0, sizeof(temp2));
				printf("\n");

			}
			else if (type == 3) {
				int from, to;
				char supplier[50];
				printf("\n\n");
				printf("---- TYPE 3 ----\n\n");
				printf("** Find that transmissions made by supplier (company name) between two given dates are defective **\n");
				printf(" Date from (YYYYMMDD)? : ");
				scanf("%d", &from);
				getchar();
				printf(" Date to   (YYYYMMDD)? : ");
				scanf("%d", &to);
				getchar();
				printf(" Supplier   ? : ");
				fgets(supplier, 50, stdin);
				supplier[strlen(supplier) - 1] = 0;
				sprintf(temp2, "select supply_date, supply_plant_name, supply_company_name from consist where supply_company_name = '%s' and supply_date between \"%d-%d-%d\" and \"%d-%d-%d\" and supply_type = \"transmission\";", supplier, from / 10000, from % 10000 / 100, from % 10000 % 100, to / 10000, to % 10000 / 100, to % 10000 % 100);
				temp = strtok(temp2, ";");
				printf("%-12s%-19ssupply_company_name\n", "supply_date", "supply_plant_name");
				while (1) {
					if (temp == NULL) {
						break;
					}
					sprintf(query, "%s", temp);
					state = mysql_query(connection, query);

					if (state == 0) {
						sql_result = mysql_store_result(connection);
						if (sql_result != NULL) {
							while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
								printf("%-12s%-19s%s\n", sql_row[0], sql_row[1], sql_row[2]);
							}
						}
					}
					temp = strtok(NULL, ";");
				}
				printf("\n---- Subtypes in TYPE 3 ----\n");
				printf("\t1. TYPE 3-1\n");
				printf("\t2. TYPE 3-2\n");
				printf("\t3  QUIT\n");
				printf("Input : ");
				scanf("%d", &go);
				getchar();
				if (go == 3) {
					memset(temp2, 0, sizeof(temp2));
					printf("\n");
					continue;
				}
				else if (go == 1) {
					printf("\nFind the VIN of each car containing such a transmission and the customer to which it was sold\n");
					memset(temp2, 0, sizeof(temp2));
					sprintf(temp2, "select supply_date, supply_plant_name, supply_company_name,VIN,C.name as customer from consist natural join Vehicle as V, customer as C where C.c_id = V.c_id and supply_company_name = '%s' and supply_date between \"%d-%d-%d\" and \"%d-%d-%d\" and supply_type = \"transmission\";", supplier, from / 10000, from % 10000 / 100, from % 10000 % 100, to / 10000, to % 10000 / 100, to % 10000 % 100);
					temp = strtok(temp2, ";");
					printf("%-12s%-19s%-20s%-20s%s\n", "supply_date", "supply_plant_name", "supply_company_name", "VIN", "customer");
					while (1) {
						if (temp == NULL) {
							break;
						}
						sprintf(query, "%s", temp);
						state = mysql_query(connection, query);

						if (state == 0) {
							sql_result = mysql_store_result(connection);
							if (sql_result != NULL) {
								while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
									printf("%-12s%-19s%-20s%-20s%s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3], sql_row[4]);
								}
							}
						}
						temp = strtok(NULL, ";");
					}
				}
				else if (go == 2) {

					printf("\nFind the dealer who sold the VIN and transmission for each vehicle containing these transmissions\n");
					memset(temp2, 0, sizeof(temp2));
					sprintf(temp2, "select supply_date, supply_plant_name, supply_company_name, name as dealer from consist natural join Vehicle natural join dealer where supply_company_name = '%s' and supply_date between \"%d-%d-%d\" and \"%d-%d-%d\" and supply_type = \"transmission\";", supplier, from / 10000, from % 10000 / 100, from % 10000 % 100, to / 10000, to % 10000 / 100, to % 10000 % 100);
					temp = strtok(temp2, ";");
					printf("%-12s%-19s%-20s%s\n", "supply_date", "supply_plant_name", "supply_company_name", "dealer");
					while (1) {
						if (temp == NULL) {
							break;
						}
						sprintf(query, "%s", temp);
						state = mysql_query(connection, query);

						if (state == 0) {
							sql_result = mysql_store_result(connection);
							if (sql_result != NULL) {
								while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
									printf("%-12s%-19s%-20s%s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3]);
								}
							}
						}
						temp = strtok(NULL, ";");
					}
				}

				memset(temp2, 0, sizeof(temp2));
				printf("\n");
			}
			else if (type == 4) {
				printf("\n\n");
				printf("---- TYPE 4 ----\n\n");
				printf("** Find the top k brands by dollar-amount sold by the year**\n");
				printf(" Which K? : ");
				scanf("%d", &K);
				getchar();

				sprintf(temp2, "create view temp as (select B.name as bname, M.m_id as mid, purchase_date, engine, color, transmission from model as M, brand as B natural join vehicle as T where M.b_id = B.b_id and M.m_id = T.m_id and T.c_id is not null ); with SALES(date, bname, price) as (select purchase_date, bname, price from temp natural join model_price where mid = m_id ), dollar(year, bname, sales) as(select year(date) as year, bname, sum(price) as dollar_amount from SALES group by year(date), price order by year(date)), order_val as( SELECT year, bname, sales, RANK() OVER( PARTITION BY year ORDER BY sales desc ) tmp_rank FROM dollar ) select* from order_val WHERE tmp_rank <= %d; ", K);
				temp = strtok(temp2, ";");
				printf("%-5s%-20s%-9s%-3s\n", "year", "brand", "sales", "rank");
				while (1) {
					if (temp == NULL) {
						break;
					}
					sprintf(query, "%s", temp);
					state = mysql_query(connection, query);

					if (state == 0) {
						sql_result = mysql_store_result(connection);
						if (sql_result != NULL) {
							while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
								printf("%-5s%-20s%-9s%-3s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3]);

							}
						}
					}
					temp = strtok(NULL, ";");
				}
				state = mysql_query(connection, "drop view temp");
				memset(temp2, 0, sizeof(temp2));
				printf("\n");

			}
			else if (type == 5) {
				printf("\n\n");
				printf("---- TYPE 5 ----\n\n");
				printf("** Find the top k brands by unit sales by the year**\n");
				printf(" Which K? : ");
				scanf("%d", &K);
				getchar();

				sprintf(temp2, "create view temp as (select B.name as bname, M.m_id as mid, purchase_date, engine, color, transmission from model as M, brand as B natural join vehicle as T where M.b_id = B.b_id and M.m_id = T.m_id and T.c_id is not null ); with sales as (select year(purchase_date) as year, bname, count(bname) as unit_sales from temp group by year(purchase_date), bname),order_val as( SELECT year, bname, unit_sales,  RANK() OVER( PARTITION BY year ORDER BY unit_sales desc ) tmp_rank FROM sales ) select year, bname, unit_sales, tmp_rank as ranking from order_val WHERE tmp_rank <= %d; ", K);
				temp = strtok(temp2, ";");
				printf("%-5s%-20s%-9s%-3s\n", "year", "brand", "sales", "rank");
				while (1) {
					if (temp == NULL) {
						break;
					}
					sprintf(query, "%s", temp);
					state = mysql_query(connection, query);

					if (state == 0) {
						sql_result = mysql_store_result(connection);
						if (sql_result != NULL) {
							while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
								printf("%-5s%-20s%-9s%-3s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3]);

							}
						}
					}
					temp = strtok(NULL, ";");
				}
				state = mysql_query(connection, "drop view temp");
				memset(temp2, 0, sizeof(temp2));
				printf("\n");
			}
			else if (type == 6) {
				printf("\n\n");
				printf("---- TYPE 6 ----\n\n");
				printf("** In what month(s) do convertibles sell best?**\n");


				sprintf(temp2, "with conv(date) as ( select purchase_date from vehicle natural join model where c_id is not null and body_style = 'Convertible' ), temp(months, count) as( select date_format(date, \"%%M\") as best_sold_months, count(date) from conv group by month(date) order by count(date) desc ) select months as best_sold_month from temp where count >= (select max(count) from temp); ");
				temp = strtok(temp2, ";");
				printf("----------------------------------\n");
				printf("%-5s\n", "best_sold_month");
				while (1) {
					if (temp == NULL) {
						break;
					}
					sprintf(query, "%s", temp);
					state = mysql_query(connection, query);

					if (state == 0) {
						sql_result = mysql_store_result(connection);
						if (sql_result != NULL) {
							while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
								printf("%-5s\n", sql_row[0]);
							}
						}
					}
					temp = strtok(NULL, ";");
				}
				memset(temp2, 0, sizeof(temp2));
				printf("\n");

			}
			else if (type == 7) {
				printf("\n\n");
				printf("---- TYPE 7 ----\n\n");
				printf("** Find those dealers who keep a vehicle in inventory for the longest average time**\n");


				sprintf(temp2, "with temp(did,cnt1,cnt2) as( select d_id, avg((case when purchase_date is null then datediff(now(), garage_date) end)) as t1, avg((case when purchase_date is not null then datediff(purchase_date, garage_date) end)) as t2 from vehicle group by d_id), temp2(did, keep) as( select did, ifnull((case when cnt1 is not null and cnt2 is not null then(cnt1 + cnt2) / 2 end), ifnull(cnt1, cnt2)) as t1 from temp) select name as longest_dealer, keep as kept_time from temp2, dealer where did = d_id and keep >= (select max(keep) from temp2); ");
				temp = strtok(temp2, ";");

				printf("----------------------------------\n");
				printf("%-11s%s\n", "dealer", "Kept_time");
				while (1) {
					if (temp == NULL) {
						break;
					}
					sprintf(query, "%s", temp);
					state = mysql_query(connection, query);

					if (state == 0) {
						sql_result = mysql_store_result(connection);
						if (sql_result != NULL) {
							while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
								printf("%-11s%s\n", sql_row[0], sql_row[1]);
							}
						}
					}
					temp = strtok(NULL, ";");
				}
				memset(temp2, 0, sizeof(temp2));
				printf("\n");

			}
			else {
				printf("Input error!\n");
			}
		}
		mysql_close(connection);
	}

	return 0;
}



