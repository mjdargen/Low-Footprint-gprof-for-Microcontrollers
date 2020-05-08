##
# Validation for Lightweight gprof for Microcontrollers
# Michael D'Argenio - mjdargen@ncsu.edu
# Created: February 28, 2020
##

##
# File formats
# debug.csv format: Type,Addr1,Addr2
# pc.csv format: Addr1,counter
# arc.csv format: Addr1,Addr2,counter
##

import pandas

inputfilename = "validation.csv"
pcfilename = "pc.csv"
arcfilename = "arc.csv"

# pc sample processing
# read in file
pcDataframe = pandas.read_csv(inputfilename)
# drop second address, not needed for pc histogram
pcDataframe = pcDataframe.drop(["Addr2"], axis=1)
# remove all call arcs
pcDataframe = pcDataframe.query('Type == "P"')
# convert from hex to dec
pcDataframe['Addr1'] = pcDataframe['Addr1'].apply(int, base=16)
# add counter column for summing pc hits
pcDataframe['Counter'] = 1
# sum up counters and remove duplicates
pcDataframe = pcDataframe.groupby(['Addr1'], as_index=False).agg({'Counter': 'sum'})
# sort values
pcDataframe = pcDataframe.sort_values(by='Addr1', ascending=True)
# convert back to hex
pcDataframe['Addr1'] = pcDataframe['Addr1'].apply(hex)
# print and write to csv
pcDataframe.to_csv(pcfilename, index=False)
print(pcDataframe.to_string(index=False))
print("\n\n")

# arc processing
# read in file
arcDataframe = pandas.read_csv(inputfilename)
# remove all pc samples
arcDataframe = arcDataframe.query('Type == "A"')
# convert from hex to dec
arcDataframe['Addr1'] = arcDataframe['Addr1'].apply(int, base=16)
arcDataframe['Addr2'] = arcDataframe['Addr2'].apply(int, base=16)
# add counter column for summing arc hits
arcDataframe['Counter'] = 1
# sum up counters and remove duplicates
arcDataframe = arcDataframe.groupby(['Addr1', 'Addr2'], as_index=False).agg({'Counter': 'sum'})
# sort values
# arcDataframe = arcDataframe.sort_values(by='Addr1', ascending=True)
arcDataframe = arcDataframe.sort_values(['Addr1', 'Addr2'], ascending=[True, True])
# convert back to hex
arcDataframe['Addr1'] = arcDataframe['Addr1'].apply(hex)
arcDataframe['Addr2'] = arcDataframe['Addr2'].apply(hex)
# print and write to csv
arcDataframe.to_csv(arcfilename, index=False)
print(arcDataframe.to_string(index=False))
