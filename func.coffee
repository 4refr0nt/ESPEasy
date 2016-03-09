# function  file
#
# for f in *.ino; do coffee func.coffee $f; done
fs = require 'fs'
args = process.argv.slice(2)

re = /(.*)\s(.*)\(.*\)(?!;)/

funcs =[]
fs.readFile args[0], (err, fd)->
  lines = fd.toString().split('\n')

  test = lines.filter (line)->
    return false if line.match /^\s*$/
    return false if line.match /\s*if/
    return false if line.match /\s*for/
    return false if line.match /\s*switch/
    return false if line.match /\s*while/
    return false if line.match /\s*case/
    return false if line.match /\s*return/
    return false if line.match /^\s*\(/
    return false if line.match /#define/
    return false if line.match /sprintf_P/
    return false if line.match /[=|+|-]/
    return false if line.match /(\s+|)\/\//
    return false if line.match /;$/
    true if line.match re

  # console.log test.join('\n')
  test.filter( (line)->
      true if line.replace /\s*/, ''
    ).map( (line)->
      line.replace(/[\s*|]\{$/,'') + ';'
    ).forEach (line)->
      funcs.push line if funcs.indexOf(line) is -1

  console.log funcs.sort().join('\n')




